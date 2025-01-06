#include "stone/AST/Decl.h"
#include "stone/AST/ASTContext.h"
#include "stone/AST/Generics.h"
#include "stone/AST/Identifier.h"
#include "stone/AST/Module.h"
#include "stone/AST/Stmt.h"
#include "stone/AST/Type.h"
#include "stone/Basic/LLVM.h"
#include "stone/Basic/LangOptions.h"
#include "stone/Basic/SrcLoc.h"

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

template <typename DeclTy, typename AllocatorTy>
void *Decl::AllocateMemory(AllocatorTy &allocatorTy, size_t baseSize,
                           bool extraSace) {
  static_assert(alignof(DeclTy) >= sizeof(void *),
                "A pointer must fit in the alignment of the DeclTy!");

  size_t size = baseSize;
  if (extraSace) {
    size += alignof(DeclTy);
  }
  void *mem = allocatorTy.AllocateMemory(size, alignof(DeclTy));
  if (extraSace)
    mem = reinterpret_cast<char *>(mem) + alignof(DeclTy);
  return mem;
}

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

// Keeping this very simple for now
bool FunDecl::IsMain() const {
  if (IsInstanceMember()) {
    return false;
  }

  return GetBasicName() == GetASTContext().GetBuiltin().BuiltinMainIdentifier;
}

/// True if the function is a defer body.
bool FunDecl::IsDeferBody() const {}

bool FunDecl::IsStatic() const { return false; }

// TODO: Remove
bool FunDecl::IsMember() const { return false; }

// TODO: Think about
bool FunDecl::IsForward() const { return false; }

bool FunDecl::HasReturn() const { return false; }

FunDecl *FunDecl::Create(ASTContext &AC, SrcLoc staticLoc, SrcLoc funLoc,
                         DeclName name, SrcLoc nameLoc, Type result,
                         DeclContext *parent) {

  size_t size = sizeof(FunDecl);
  // + (HasImplicitThisDecl ? sizeof(ParamDecl *) : 0);
  void *funDeclPtr = Decl::AllocateMemory<FunDecl>(AC, size);
  auto FD = ::new (funDeclPtr)
      FunDecl(DeclKind::Fun, staticLoc, funLoc, name, nameLoc, result, parent);
  return FD;
}

// FunDecl *FunDecl::CreateImplicit(DeclSpecifierCollector &collector,
//                                  ASTContext &sc, DeclContext *parent) {

//   return nullptr;
// }

// StructDecl *StructDecl::Create(DeclName name, SrcLoc loc,
//                                ASTContext &astContext, DeclContext *dc) {

//   size_t size = sizeof(StructDecl);
//   auto declPtr = Decl::AllocateMemory<StructDecl>(astContext, size);
//   // return ::new (declPtr) StructDecl(loc, GetASTContext(), dc);
//   return nullptr;
// }

ModuleDecl *ModuleDecl::Create(Identifier name, ASTContext &astContext) {
  size_t size = sizeof(ModuleDecl);
  auto declPtr = Decl::AllocateMemory<ModuleDecl>(astContext, size);
  return ::new (declPtr) ModuleDecl(name, astContext);
}

ModuleDecl *ModuleDecl::CreateMainModule(Identifier name,
                                         ASTContext &astContext) {
  auto mainModuleDecl = ModuleDecl::Create(name, astContext);
  mainModuleDecl->Bits.ModuleDecl.IsMainModule = true;
  return mainModuleDecl;
}

VarDecl *VarDecl::Create(ASTContext &astContext) {
  // auto declPtr = stone::AllocateMem<VarDecl>(astContext,
  // sizeof(VarDecl)); return ::new (declPtr) VarDecl(astContext);
  return nullptr;
}
