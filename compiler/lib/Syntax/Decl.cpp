#include "stone/Syntax/Decl.h"
#include "stone/Basic/LLVM.h"
#include "stone/Basic/LangOptions.h"
#include "stone/Basic/SrcLoc.h"
#include "stone/Syntax/Identifier.h"
#include "stone/Syntax/Module.h"
#include "stone/Syntax/Stmt.h"
#include "stone/Syntax/SyntaxContext.h"
#include "stone/Syntax/SyntaxFactory.h"
#include "stone/Syntax/Template.h" //DeclTemplate
#include "stone/Syntax/Types.h"

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
using namespace stone::syn;

template <typename DeclTy, typename AllocatorTy>
void *syn::AllocateDeclMem(AllocatorTy &allocatorTy, size_t baseSize,
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
// void *syn::Decl::operator new(std::size_t bytes, const SyntaxContext &tc,
//                               unsigned alignment) {
//   return tc.Allocate(bytes, alignment);
// }

// // Only allow allocation of Modules using the allocator in ASTContext.
// void *syn::Module::operator new(std::size_t bytes, const SyntaxContext &tc,
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

// TODO: Set body  -- not being set now.
void FunctionDecl::SetBody(BraceStmt *body, BodyStatus bodyStatus) {
  SetBodyStatus(bodyStatus);
}

template <std::size_t len>
static bool IsMainImpl(const NameableDecl *nameable, const char (&str)[len]) {
  assert(nameable);
  auto identifier = nameable->GetIdentifier();
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

void FunDecl::SetFunLoc(SrcLoc loc) { funLoc = loc; }

void DeclStats::Print(ColorfulStream &stream) {}

FunDecl *FunDeclFactory::Create(DeclCollector &collector, SyntaxContext &sc,
                                TypeRep *result, DeclContext *parent) {
  size_t size = sizeof(FunDecl);
  // + (HasImplicitThisDecl ? sizeof(ParamDecl *) : 0);

  auto memPtr = syn::AllocateDeclMem<FunDecl>(sc, size);
  return ::new (memPtr) FunDecl(
      DeclKind::Fun, collector.GetFunctionSpecifierCollector().GetFunLoc(),
      collector.GetDeclName(), collector.GetDeclNameLoc(), parent);
}

StructDecl *StructDeclFactory::Create(DeclName name, SrcLoc loc,
                                      SyntaxContext &sc, DeclContext *dc) {
  size_t size = sizeof(StructDecl);
  auto declPtr = syn::AllocateDeclMem<StructDecl>(sc, size);
  // return ::new (declPtr) StructDecl(loc, GetSyntaxContext(), dc);
  return nullptr;
}

Module *ModuleDeclFactory::Create(Identifier name, SyntaxContext &sc,
                                  bool isMainModule) {
  auto declPtr = syn::AllocateDeclMem<syn::Module>(sc, sizeof(syn::Module));
  return ::new (declPtr) syn::Module(name, sc);
}

VarDecl *VarDeclFactory::Create(SyntaxContext &sc) {
  // auto declPtr = syn::AllocateDeclMem<syn::VarDecl>(sc,
  // sizeof(syn::VarDecl)); return ::new (declPtr) syn::VarDecl(sc);
  return nullptr;
}
