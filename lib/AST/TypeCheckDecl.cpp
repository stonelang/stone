#include "stone/AST/ASTVisitor.h"
#include "stone/AST/TypeChecker.h"

using namespace stone;

class DeclChecker final : public DeclVisitor<DeclChecker> {

  ASTContext &AC;
  SourceFile *SF;

public:
  DeclChecker(ASTContext &AC, SourceFile *SF) : AC(AC), SF(SF) {}

public:
  void Visit(Decl *D) {
    // DeclVisitor<DeclChecker>::Visit(D);
    //  checker.CheckDecl(D);
  }

public:
  void VisitFunDecl(FunDecl *funDecl) {
    TypeChecker::CheckVisibilityLevel(funDecl);
  }

  // void VisitImportDecl(ImportDecl *importDecl) {}
  // void VisitIfConfigDecl(IfConfigDecl *ifConfigDecl) {}
};

bool TypeChecker::CheckTopLevelDecls() {

  // for (auto topLevelDecl : sourceFile.topLevelDecls) {
  //   if (!CheckTopLevelDecl(topLevelDecl)) {

  //   sourceFile.SetTypeCheckedStage();
  // }
}

bool TypeChecker::CheckTopLevelDecl(Decl *topLevelDecl) { return true; }

bool TypeChecker::CheckDecl(Decl *D) {

  // auto *SF = D->GetDeclContext()->GetParentSourceFile();
  // DeclChecker(*this, D->GetASTContext(), SF).Visit(d);
}
