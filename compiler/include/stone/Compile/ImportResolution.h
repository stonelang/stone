#ifndef STONE_SEM_IMPORTRESOLUTION_H
#define STONE_SEM_IMPORTRESOLUTION_H

namespace stone {
namespace syn {
class ASTFile;
} // namespace syn

namespace sem {
// TODO: ModuleResolution
void ResolveImports(syn::ASTFile &syntaxFile);
} // namespace sem
} // namespace stone

#endif
