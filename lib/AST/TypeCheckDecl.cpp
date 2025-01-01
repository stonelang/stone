#include "stone/AST/ASTVisitor.h"
#include "stone/AST/TypeChecker.h"
#include "stone/AST/Visibility.h"

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
    stone::CheckVisibilityControl(FD);

    // TODO:
    // TypeChecker::CheckParameterList(FD->GetParameters(), FD);
  }

  void VisitStructDecl(StructDecl *structDecl) {}
};

bool TypeChecker::CheckDecl(Decl *D) {
  auto *SF = D->GetDeclContext()->GetParentSourceFile();
  DeclChecker(D->GetASTContext(), SF).Visit(D);
  return true;
}
