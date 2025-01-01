#include "stone/AST/ASTVisitor.h"
#include "stone/AST/TypeChecker.h"

using namespace stone;

class DeclChecker final : public DeclVisitor<DeclChecker> {

  ASTContext &AC;
  SourceFile *SF;

public:
  DeclChecker(ASTContext &AC, SourceFile *SF) : AC(AC), SF(SF) {}

public:
  void Visit(Decl *D) { DeclVisitor<DeclChecker>::Visit(D); }

public:
  void VisitFunDecl(FunDecl *FD) {
    TypeChecker::CheckVisibilityLevel(FD);

    // TODO:
    // TypeChecker::CheckParameterList(FD->GetParameters(), FD);
  }

  void VisitStructDecl(StructDecl *structDecl) {}
};

bool TypeChecker::CheckTopLevelDecls() {
  for (auto topLevelDecl : sourceFile.topLevelDecls) {
    if (!CheckTopLevelDecl(topLevelDecl)) {
      return false;
    }
    sourceFile.SetTypeCheckedStage();
  }
}

bool TypeChecker::CheckTopLevelDecl(Decl *topLevelDecl) {
  /// Just return for now.
  return CheckDecl(topLevelDecl);
}

bool TypeChecker::CheckDecl(Decl *D) {
  auto *SF = D->GetDeclContext()->GetParentSourceFile();
  DeclChecker(D->GetASTContext(), SF).Visit(D);
  return true;
}
