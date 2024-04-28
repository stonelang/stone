#include "stone/Sem/TypeChecker.h"
#include "stone/AST/ASTWalker.h"
#include "stone/AST/TypeVisitor.h"

using namespace stone;

class TypeChecking final : public TypeVisitor<TypeChecking>, public ASTWalker {
public:
};

void TypeChecker::CheckTypes(Decl *d) {

  // if (!decl || decl->IsInvalid()){
  //     return;
  // }
  // auto sourceFile = d->GetDeclContext()->GetParentSourceFile();
  // if (sourceFile && sourceFile->GetKind() == SourceFileKind::Interface) {
  //   return;
  // }
  auto &ctx = d->GetASTContext();
}

void TypeChecker::CheckTypes(Stmt *stmt, DeclContext *dc) {}

void TypeChecker::CheckTypes(AliasDecl *alias) {}

void TypeChecker::CheckTypes(TrailingWhereClause *whereClause) {}

void TypeChecker::CheckTypes(GenericParamList *params) {}