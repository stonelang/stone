#include "stone/AST/Module.h"
#include "stone/CodeAna/ImportResolution.h"

using namespace stone;
using namespace stone::ast;

// TODO:
//  class UseResolver final : public DeclVisitor<UseResolver> {
//    friend DeclVisitor<UseResolver>;

//   ASTFile &SF;
//   ASTContext &ctx;
// };

void codeana::ResolveImports(ASTFile &asttaxFile) {}