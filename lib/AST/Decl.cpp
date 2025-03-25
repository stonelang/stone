#include "stone/AST/Decl.h"
#include "stone/AST/ASTContext.h"
#include "stone/AST/Identifier.h"
#include "stone/AST/Module.h"
#include "stone/AST/Property.h"
#include "stone/AST/Stmt.h"
#include "stone/AST/Template.h"
#include "stone/AST/Type.h"
#include "stone/AST/TypeState.h"
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

Decl::Decl(DeclKind kind, DeclState *declState, UnifiedContext context)
    : kind(kind), declState(declState), context(context) {}

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

bool ValueDecl::IsPublic() const {
  return GetVisibilityLevel() == VisibilityLevel::Public;
}

bool ValueDecl::IsPrivate() const {
  return GetVisibilityLevel() == VisibilityLevel::Private;
}

bool ValueDecl::IsInternal() const {
  return GetVisibilityLevel() == VisibilityLevel::Internal;
}

bool ValueDecl::HasVisibilityLevel() const {
  return GetState()->GetDeclPropertyList().HasProperty(
      PropertyKind::Visibility);
}

VisibilityLevel ValueDecl::GetVisibilityLevel() const {
  if (HasVisibilityLevel()) {
    auto vm = static_cast<VisibilityModifier *>(
        GetState()->GetDeclPropertyList().GetProperty(
            PropertyKind::Visibility));
    return vm->GetVisibilityLevel();
  }
  return VisibilityLevel::None;
}

void ValueDecl::ChangeVisibility(VisibilityLevel level) {
  if (HasVisibilityLevel()) {
    auto vm = static_cast<VisibilityModifier *>(
        GetState()->GetDeclPropertyList().GetProperty(
            PropertyKind::Visibility));
    return vm->SetVisibilityLevel(level);
  }
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

TemplateContext::TemplateContext(DeclContextKind kind, DeclContext *parent,
                                 TemplateParamList *params)
    : TemplateContextBase(), DeclContext(kind, parent) {

  // TODO:
  // if (params) {
  //   params->SetDeclContext(this);
  //   cenericParamsAndState.setPointerAndInt(params,
  //   TemplateParamsState::Parsed);
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

bool FunDecl::IsStatic() const {
  return GetState()->GetDeclPropertyList().HasProperty(PropertyKind::Static);
}

// TODO: Remove
bool FunDecl::IsMember() const { return false; }

// TODO: Think about
bool FunDecl::IsForward() const { return false; }

bool FunDecl::HasReturn() const { return false; }

FunDecl *FunDecl::Create(DeclState *DS, DeclContext *parent) {

  size_t size = sizeof(FunDecl);
  // + (HasImplicitThisDecl ? sizeof(ParamDecl *) : 0);
  void *funDeclPtr = Decl::AllocateMemory<FunDecl>(DS->GetASTContext(), size);
  auto FD =
      ::new (funDeclPtr) FunDecl(DeclKind::Fun, DS, parent); // TODO: DeclState
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
  return ::new (declPtr) ModuleDecl(nullptr, astContext); // TODO: DeclState
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

DeclState::DeclState(ASTContext &astContext)
    : astContext(astContext), declPropertyList(astContext),
      typePropertyList(astContext) {}
