#ifndef STONE_SEM_IMPORTRESOLUTION_H
#define STONE_SEM_IMPORTRESOLUTION_H

namespace stone {
namespace syn {
class SyntaxFile;
} // namespace syn

namespace sem {
// TODO: ModuleResolution
void ResolveImports(syn::SyntaxFile &syntaxFile);
} // namespace sem
} // namespace stone

#endif
