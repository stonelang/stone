#include "stone/AST/Module.h"
#include "stone/Public.h"

using namespace stone;

// TODO:
//  class UseResolver final : public DeclVisitor<UseResolver> {
//    friend DeclVisitor<UseResolver>;

//   SourceFile &SF;
//   ASTContext &ctx;
// };

void stone::ResolveSourceFileImports(SourceFile &sourceFile) {}