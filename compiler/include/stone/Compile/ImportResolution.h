#ifndef STONE_SEM_IMPORTRESOLUTION_H
#define STONE_SEM_IMPORTRESOLUTION_H

namespace stone {
namespace ast {
class ASTFile;
} // namespace ast

namespace sem {
// TODO: ModuleResolution
void ResolveImports(ast::ASTFile &asttaxFile);
} // namespace sem
} // namespace stone

#endif
