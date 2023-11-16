#include "stone/Public.h"
#include "stone/Syntax/Module.h"

using namespace stone;
using namespace stone::syn;

// TODO:
//  class UseResolver final : public DeclVisitor<UseResolver> {
//    friend DeclVisitor<UseResolver>;

//   ASTFile &SF;
//   ASTContext &ctx;
// };

void stone::ResolveASTFileImports(syn::ASTFile &astFile) {}