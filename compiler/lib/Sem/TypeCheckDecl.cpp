#include "stone/Sem/TypeChecker.h"
#include "stone/Syntax/SyntaxVisitor.h"

using stone::sem::TypeChecker;

class AccessLevelCheckingBase {
public:
};

class AccessLevelChecking : public AccessLevelCheckingBase,
                            public DeclVisitor<AccessLevelChecking> {
public:
  void Visit(Decl *d) {
    // if (d->IsInvalid() || d->IsImplicit()) {
    //   return;
    // }
    DeclVisitor<AccessLevelChecking>::Visit(d);
  }
  // Force all kinds to be handled at a lower level.
  void VisitDecl(Decl *D) {}
  void VisitValueDecl(ValueDecl *D) {}
  void VisitIfConfigDecl(IfConfigDecl *ifConfigDecl) {}

  // #define UNREACHABLE(KIND, REASON) \
  // void visit##KIND##Decl(KIND##Decl *D) { \
  //   llvm_unreachable(REASON); \
  // }
  // #undef UNREACHABLE
};

class DeclChecking final : public DeclVisitor<DeclChecking> {
  TypeChecker &checker;
  SyntaxContext &sc;
  SyntaxFile *sf;

public:
  DeclChecking(TypeChecker &checker, SyntaxContext &sc, SyntaxFile *sf)
      : checker(checker), sc(sc), sf(sf) {}

public:
  void Visit(Decl *d) {
    //
    DeclVisitor<DeclChecking>::Visit(d);

    //checker.CheckTypes(d);
  }
  void VisitDecl(Decl *D) {}

public:
  void VisitFunDecl(FunDecl *funDecl) { checker.CheckAccessLevel(funDecl); }
  
  void VisitImportDecl(ImportDecl *importDecl) {}
  void VisitIfConfigDecl(IfConfigDecl *ifConfigDecl) {}
};

void TypeChecker::CheckDecl(Decl *d) {

  auto *sf = d->GetDeclContext()->GetParentSyntaxFile();
  DeclChecking(*this, d->GetSyntaxContext(), sf).Visit(d);
}

void TypeChecker::CheckAccessLevel(Decl *d) {

  if (llvm::isa<syn::ValueDecl>(d)) {
    // AccessControlChecker().visit(D);
    // UsableFromInlineChecker().visit(D);
  }

  // if (llvm::isa<syn::ValueDecl>(D) || isa<PatternBindingDecl>(D)) {
  //   bool allowInlineable =
  //       D->getDeclContext()->isInSpecializeExtensionContext();
  //   AccessControlChecker(allowInlineable).visit(D);
  //   UsableFromInlineChecker().visit(D);
  // } else if (auto *ED = dyn_cast<ExtensionDecl>(D)) {
  //   checkExtensionGenericParamAccess(ED);
  // }
}

// void TypeChecker::CheckValueDecl(ValueDecl *d) {

//   if (llvm::isa<syn::ValueDecl>(d)) {
//     // AccessControlChecker().visit(D);
//     // UsableFromInlineChecker().visit(D);
//   }

//   // switch (d->GetKind()) {
//   // case DeclKind::Import:{
//   //   llvm_unreachable("Not a 'ValueDecl' ");
//   // }
//   // case DeclKind::Fun:{
//   //   auto *fd = llvm::cast<FunDecl>(d);

//   //   ComputeAccessLevel(fd);
//   // }

// }

// void TypeChecker::FinalizeValueDecl(ValueDecl *d) {}

// void TypeChecker::ComputeAccessLevel(ValueDecl *d) {}
