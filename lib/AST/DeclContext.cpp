#include "stone/AST/DeclContext.h"
#include "stone/AST/ASTContext.h"
#include "stone/AST/Module.h"
#include "stone/AST/Visibility.h"
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

using namespace stone;

DeclContext::DeclContext(DeclContextKind declContextKind, DeclContext *parent)
    : declContextKind(declContextKind), parent(parent) {

  if (IsNot(DeclContextKind::ModuleDecl)) {
    assert(parent != nullptr && "DeclContext must have a parent DeclContext!");
  }
}

DeclContextKind DeclContext::GetDeclContextKind() const {
  return declContextKind;
}

ASTContext &DeclContext::GetASTContext() const {
  return GetParentModule()->GetASTContext();
}

ModuleDecl *DeclContext::GetParentModule() const {
  const DeclContext *dc = this;
  while (!dc->IsModuleContext()) {
    dc = dc->GetParent();
  }
  return const_cast<ModuleDecl *>(llvm::cast<ModuleDecl>(dc));
}

SourceFile *DeclContext::GetParentSourceFile() const {
  const DeclContext *dc = this;
  while (!dc->IsModuleFileContext()) {
    dc = dc->GetParent();
  }
  return const_cast<SourceFile *>(llvm::dyn_cast<SourceFile>(dc));
}

DeclContext *Decl::GetDeclContextForModule() const {
  if (auto module = dyn_cast<ModuleDecl>(this)) {
    return const_cast<ModuleDecl *>(module);
  }
  return nullptr;
}

bool DeclContext::IsTypeContext() const {
  if (auto decl = ToDecl()) {
    return isa<NominalTypeDecl>(decl);
  }
  return false;
}

VisibilityScope::VisibilityScope(const DeclContext *DC, bool isPrivate)
    : val(DC, isPrivate) {
  // if (isPrivate) {
  //   DC = getPrivateDeclContext(DC, DC->getParentSourceFile());
  //   Value.setPointer(DC);
  // }
  // if (!DC || isa<ModuleDecl>(DC))
  //   assert(!isPrivate && "public or internal scope can't be private");
}

bool VisibilityScope::IsFileScope() const {
  // auto DC = getDeclContext();
  // return DC && isa<FileUnit>(DC);
  assert(false && "Not implemented");
}

bool VisibilityScope::IsInternal() const {
  // auto DC = getDeclContext();
  // return DC && isa<ModuleDecl>(DC);
  assert(false && "Not implemented");
}

VisibilityLevel VisibilityScope::GetVisibilityLevelForDiagnostics() const {
  assert(false && "Not implemented");
  // if (isPublic())
  //   return VisibilityLevel::Public;
  // if (isa<ModuleDecl>(getDeclContext()))
  //   return VisibilityLevel::Internal;
  // if (getDeclContext()->isModuleScopeContext()) {
  //   return isPrivate() ? VisibilityLevel::Private :
  //   VisibilityLevel::FilePrivate;
  // }

  return VisibilityLevel::Private;
}

bool VisibilityScope::AllowsPrivateVisibility(const DeclContext *useDC,
                                              const DeclContext *sourceDC) {
  // // Check the lexical scope.
  // if (useDC->isChildContextOf(sourceDC))
  //   return true;

  // // Do not allow access if the sourceDC is in a different file
  // auto useSF = useDC->getParentSourceFile();
  // if (useSF != sourceDC->getParentSourceFile())
  //   return false;

  // // Do not allow access if the sourceDC does not represent a type.
  // auto sourceNTD = sourceDC->getSelfNominalTypeDecl();
  // if (!sourceNTD)
  //   return false;

  // // Compare the private scopes and iterate over the parent types.
  // sourceDC = getPrivateDeclContext(sourceDC, useSF);
  // while (!useDC->isModuleContext()) {
  //   useDC = getPrivateDeclContext(useDC, useSF);
  //   if (useDC == sourceDC)
  //     return true;

  //   // Get the parent type. If the context represents a type, look at the
  //   types
  //   // declaring context instead of the contexts parent. This will crawl up
  //   // the type hierarchy in nested extensions correctly.
  //   if (auto NTD = useDC->getSelfNominalTypeDecl())
  //     useDC = NTD->getDeclContext();
  //   else
  //     useDC = useDC->getParent();
  // }
  assert(false && "Not implemented");
}
