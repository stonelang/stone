#include "stone/Sem/TypeChecker.h"
#include "stone/Syntax/SyntaxWalker.h"
#include "stone/Syntax/TypeVisitor.h"

using namespace stone::sem;
using namespace stone::syn;

class TypeChecking final : public TypeVisitor<TypeChecking>,
                           public SyntaxWalker {
public:
};

void TypeChecker::CheckTypes(Decl *d) {

  // if (!decl || decl->IsInvalid()){
  //     return;
  // }
  // auto syntaxFile = d->GetDeclContext()->GetParentSourceFile();
  // if (syntaxFile && syntaxFile->GetKind() == SyntaxFileKind::Interface) {
  //   return;
  // }

  auto &ctx = d->GetSyntaxContext();
}

void TypeChecker::CheckTypes(Stmt *stmt, DeclContext *dc) {}

void TypeChecker::CheckTypes(AliasDecl *alias) {}

void TypeChecker::CheckTypes(TrailingWhereClause *whereClause) {}

void TypeChecker::CheckTypes(GenericParamList *params) {}