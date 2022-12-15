#include "stone/Syntax/DeclContext.h"

#include "stone/Basic/LLVM.h"
#include "stone/Basic/LangOptions.h"
#include "stone/Basic/SrcLoc.h"
#include "stone/Syntax/Module.h"
#include "stone/Syntax/SyntaxContext.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/MathExtras.h"
#include "llvm/Support/VersionTuple.h"
#include "llvm/Support/raw_ostream.h"

using namespace stone;
using namespace stone::syn;

DeclContext::DeclContext(DeclContextKind declContextKind, DeclContext *parent)
    : declContextKind(declContextKind), parent(parent), 
      syntaxHierarchyKind(GetSyntaxHierarchyFromKind(declContextKind)) {

  if (declContextKind != DeclContextKind::Module) {
    assert(parent != nullptr && "DeclContext must have a parent context");
  }
}

DeclContextKind DeclContext::GetDeclContextKind() const {

  // switch (parentAndKind.getInt()) {
  // case SyntaxHierarchy::ModuleFile:
  //   return DeclContextKind::ModuleFile;
  // case SyntaxHierarchy::Decl: {
  //   auto decl = reinterpret_cast<const Decl *>(this + 1);
  //   if (llvm::isa<FunctionDecl>(decl))
  //     return DeclContextKind::FunctionDecl;
  //   // if (llvm::isa<TemplateTypeDecl>(decl))
  //   //   return DeclContextKind::GenericTypeDecl;
  //   switch (decl->GetKind()) {
  //   case DeclKind::Module:
  //     return DeclContextKind::Module;
  //   default:
  //     llvm_unreachable("Unhandled Decl kind");
  //   }
  // }
  // }
  // llvm_unreachable("Unhandled DeclContext SyntaxHierarchy");
  return declContextKind;
}

SyntaxContext &DeclContext::GetSyntaxContext() const {
  return GetParentModule()->GetSyntaxContext();
}

syn::ModuleDecl *DeclContext::GetParentModule() const {
  const DeclContext *dc = this;
  while (!dc->IsModuleContext()) {
    dc = dc->GetParent();
  }
  return const_cast<ModuleDecl *>(llvm::cast<ModuleDecl>(dc));
}

DeclContext *Decl::GetDeclContextForModule() const {
  if (auto module = dyn_cast<syn::ModuleDecl>(this)) {
    return const_cast<syn::ModuleDecl *>(module);
  }
  return nullptr;
}

bool DeclContext::IsTypeContext() const {
  if (auto decl = ToDecl()) {
    return isa<NominalTypeDecl>(decl);
  }
  return false;
}
