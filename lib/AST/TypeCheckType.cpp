#include "stone/AST/ASTWalker.h"
#include "stone/AST/TypeChecker.h"
#include "stone/AST/TypeVisitor.h"

using namespace stone;

class TypeCheckerType final : public TypeVisitor<TypeCheckerType>,
                              public ASTWalker {
public:
};

// void TypeChecker::CheckType(Decl *d) {

//   // if (!decl || decl->IsInvalid()){
//   //     return;
//   // }
//   // auto sourceFile = d->GetDeclContext()->GetParentSourceFile();
//   // if (sourceFile && sourceFile->GetKind() == SourceFileKind::Interface) {
//   //   return;
//   // }
//   auto &ctx = d->GetASTContext();
// }

// void TypeChecker::CheckTypes(Stmt *stmt, DeclContext *dc) {}

// void TypeChecker::CheckTypes(AliasDecl *alias) {}

// void TypeChecker::CheckTypes(TrailingWhereClause *whereClause) {}

// void TypeChecker::CheckTypes(TemplateParamList *params) {}