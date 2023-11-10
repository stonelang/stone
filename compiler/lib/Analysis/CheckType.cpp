#include "stone/AST/ASTWalker.h"
#include "stone/AST/TypeVisitor.h"
#include "stone/Analysis/TypeChecker.h"

using namespace stone;

class TypeChecking final : public TypeVisitor<TypeChecking>, public ASTWalker {
public:
};

void TypeChecker::CheckTypes(Decl *d) {

  // if (!decl || decl->IsInvalid()){
  //     return;
  // }
  // auto asttaxFile = d->GetDeclContext()->GetParentSourceFile();
  // if (asttaxFile && asttaxFile->GetKind() == ASTFileKind::Interface) {
  //   return;
  // }

  auto &ctx = d->GetASTContext();
}

void TypeChecker::CheckTypes(Stmt *stmt, DeclContext *dc) {}

void TypeChecker::CheckTypes(AliasDecl *alias) {}

void TypeChecker::CheckTypes(TrailingWhereClause *whereClause) {}

void TypeChecker::CheckTypes(GenericParamList *params) {}