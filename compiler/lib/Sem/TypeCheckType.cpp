#include "stone/Sem/TypeChecker.h"
#include "stone/Syntax/ASTWalker.h"
#include "stone/Syntax/TypeVisitor.h"

using namespace stone;

class TypeChecking final : public TypeVisitor<TypeChecking>, public ASTWalker {
public:
};

void TypeChecker::CheckTypes(Decl *d) {

  // if (!decl || decl->IsInvalid()){
  //     return;
  // }
  // auto astFile = d->GetDeclContext()->GetParentSourceFile();
  // if (astFile && astFile->GetKind() == ASTFileKind::Interface) {
  //   return;
  // }
  auto &ctx = d->GetASTContext();
}

void TypeChecker::CheckTypes(Stmt *stmt, DeclContext *dc) {}

void TypeChecker::CheckTypes(AliasDecl *alias) {}

void TypeChecker::CheckTypes(TrailingWhereClause *whereClause) {}

void TypeChecker::CheckTypes(GenericParamList *params) {}