#include "stone/AST/Decl.h"
#include "stone/Basic/LLVM.h"
#include "stone/Basic/LangOptions.h"
#include "stone/Basic/SrcLoc.h"
#include "stone/AST/DeclFactory.h"
#include "stone/AST/Generics.h"
#include "stone/AST/Identifier.h"
#include "stone/AST/Module.h"
#include "stone/AST/Stmt.h"
#include "stone/AST/ASTContext.h"
#include "stone/AST/Types.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/MathExtras.h"
#include "llvm/Support/VersionTuple.h"
#include "llvm/Support/raw_ostream.h"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <string>
#include <tuple>
#include <utility>

using namespace stone;
using namespace stone::ast;

template <typename DeclTy, typename AllocatorTy>
void *ast::AllocateDeclMem(AllocatorTy &allocatorTy, size_t baseSize,
                           bool extraSace) {
  static_assert(alignof(DeclTy) >= sizeof(void *),
                "A pointer must fit in the alignment of the DeclTy!");

  size_t size = baseSize;
  if (extraSace) {
    size += alignof(DeclTy);
  }
  void *mem = allocatorTy.Allocate(size, alignof(DeclTy));
  if (extraSace)
    mem = reinterpret_cast<char *>(mem) + alignof(DeclTy);
  return mem;
}

// // Only allow allocation of Decls using the allocator in ASTContext.
// void *ast::Decl::operator new(std::size_t bytes, const ASTContext &tc,
//                               unsigned alignment) {
//   return tc.Allocate(bytes, alignment);
// }

// // Only allow allocation of Modules using the allocator in ASTContext.
// void *ast::Module::operator new(std::size_t bytes, const ASTContext &tc,
//                                 unsigned alignment) {
//   return tc.Allocate(bytes, alignment);
// }

bool ValueDecl::IsInstanceMember() const {
  DeclContext *dc = GetDeclContext();

  if (!dc->IsTypeContext()) {
    return false;
  }

  switch (GetKind()) {
    // case DeclKind::Import:
    // case DeclKind::Extension:
    // case DeclKind::PatternBinding:
    // case DeclKind::EnumCase:
    // case DeclKind::TopLevelCode:
    // case DeclKind::InfixOperator:
    // case DeclKind::PrefixOperator:
    // case DeclKind::PostfixOperator:
    // case DeclKind::IfConfig:
    // case DeclKind::PoundDiagnostic:
    // case DeclKind::PrecedenceGroup:
    // case DeclKind::MissingMember:
    //   llvm_unreachable("Not a ValueDecl");

  case DeclKind::Struct:
  case DeclKind::Enum:
  case DeclKind::Interface:
    // case DeclKind::Alias:
    //  Types are not instance members.
    return false;

    // case DeclKind::Constructor:
    //   // Constructors are not instance members.
    //   return false;

    // case DeclKind::Destructor:
    //   // Destructors are technically instance members, although they
    //   // can't actually be referenced as such.
    //   return true;

  case DeclKind::Fun:
    // Non-static methods are instance members.
    // return !cast<FunDecl>(this)->IsStatic(); // TODO:
    return true;

    // case DeclKind::Param:
    //   // enum elements and function parameters are not instance members.
    //   return false;

    // case DeclKind::Subscript:
    // case DeclKind::Var:
    //   // Non-static variables and subscripts are instance members.
    //   return !cast<AbstractStorageDecl>(this)->isStatic();

    // case DeclKind::Module:
    //   // Modules are never instance members.
    //   return false;
  }
  llvm_unreachable("bad DeclKind");
}

// void Decl::SetInvalid() {
//   switch (GetKind()) {
// #define VALUE_DECL(ID, PARENT)
// #define DECL(ID, PARENT) \
//   case DeclKind::ID:
// #include "stone/AST/DeclKind.def"
//     Bits.Decl.Invalid = true;
//     return;
//   case DeclKind::Enum:
//   case DeclKind::Struct:
//   case DeclKind::Interface:
//   case DeclKind::Alias:
//   case DeclKind::Module:
//   case DeclKind::Var:
//   case DeclKind::Param:
//   case DeclKind::Constructor:
//   case DeclKind::Destructor:
//   case DeclKind::Fun:
//     //llvm::cast<ValueDecl>(this)->SetInterfaceType(ErrorType::Get(GetASTContext()));
//     return;

//   case DeclKind::BuiltinTuple:
//     llvm_unreachable("BuiltinTupleDecl should not end up here");
//   }

//   llvm_unreachable("Unknown decl kind");
// }

GenericContext::GenericContext(DeclContextKind kind, DeclContext *parent,
                               GenericParamList *params)
    : GenericContextBase(), DeclContext(kind, parent) {

  // TODO:
  // if (params) {
  //   params->SetDeclContext(this);
  //   cenericParamsAndState.setPointerAndInt(params,
  //   GenericParamsState::Parsed);
  // }
}

// TODO: Set body  -- not being set now.
void FunctionDecl::SetBody(BraceStmt *body, BodyStatus bodyStatus) {
  SetBodyStatus(bodyStatus);
}

template <std::size_t len>
static bool IsMainImpl(const NameableDecl *nameable, const char (&str)[len]) {
  assert(nameable);
  auto identifier = nameable->GetBasicName();
  return identifier.IsEqual(str);
}

// Keeping this very simple for now
bool FunDecl::IsMain() const {
  return (!IsInstanceMember() && IsMainImpl(this, "Main"));
}

/// True if the function is a defer body.
bool FunDecl::IsDeferBody() const {}

bool FunDecl::IsStatic() const { return false; }

// TODO: Remove
bool FunDecl::IsMember() const { return false; }

// TODO: Think about
bool FunDecl::IsForward() const { return false; }

bool FunDecl::HasReturn() const { return false; }

void DeclStats::Print(ColorStream &stream) {}

FunDecl *FunDecl::Create(DeclCollector &collector, ASTContext &sc, DeclContext *parent) {
  size_t size = sizeof(FunDecl);
  // + (HasImplicitThisDecl ? sizeof(ParamDecl *) : 0);

  auto memPtr = ast::AllocateDeclMem<FunDecl>(sc, size);
  auto funDecl = ::new (memPtr) FunDecl(
      DeclKind::Fun, collector.GetFunctionSpecifierCollector().GetFunLoc(),
      collector.GetDeclName(), collector.GetDeclNameLoc(),
      collector.GetTypeCollector().GetType(), parent);

  funDecl->SetAccessLevel(collector.GetAccessLevelCollector().GetAccessLevel());
  return funDecl;
}

StructDecl *DeclFactory::MakeStructDecl(DeclName name, SrcLoc loc,
                                        ASTContext &sc, DeclContext *dc) {
  size_t size = sizeof(ast::StructDecl);
  auto declPtr = ast::AllocateDeclMem<StructDecl>(sc, size);
  // return ::new (declPtr) StructDecl(loc, GetASTContext(), dc);
  return nullptr;
}

ModuleDecl *DeclFactory::MakeModuleDecl(Identifier name, ASTContext &sc,
                                        bool isMainModule) {
  size_t size = sizeof(ast::ModuleDecl);
  auto declPtr = ast::AllocateDeclMem<ast::ModuleDecl>(sc, size);
  return ::new (declPtr) ast::ModuleDecl(name, sc);
}

ModuleDecl *ModuleDecl::Create(Identifier name, ASTContext &sc, bool isMainModule) {
  size_t size = sizeof(ast::ModuleDecl);
  auto declPtr = ast::AllocateDeclMem<ast::ModuleDecl>(sc, size);
  return ::new (declPtr) ast::ModuleDecl(name, sc);
}

StructDecl *StructDecl::Create(DeclName name, SrcLoc loc,
                                        ASTContext &sc, DeclContext *dc) {
  size_t size = sizeof(ast::StructDecl);
  auto declPtr = ast::AllocateDeclMem<StructDecl>(sc, size);
  // return ::new (declPtr) StructDecl(loc, GetASTContext(), dc);
  return nullptr;
}

VarDecl *VarDecl::Create(ASTContext &sc) {
  // auto declPtr = ast::AllocateDeclMem<ast::VarDecl>(sc,
  // sizeof(ast::VarDecl)); return ::new (declPtr) ast::VarDecl(sc);
  return nullptr;
}
