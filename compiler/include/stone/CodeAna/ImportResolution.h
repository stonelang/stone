#ifndef STONE_CODEANA_IMPORTRESOLUTION_H
#define STONE_CODEANA_IMPORTRESOLUTION_H

namespace stone {
namespace ast {
class ASTFile;
} // namespace ast

namespace codeana {
// TODO: ModuleResolution
void ResolveImports(ast::ASTFile &asttaxFile);
} // namespace codeana
} // namespace stone

#endif
