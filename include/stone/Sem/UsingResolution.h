#ifndef STONE_SEM_USINGRESOLUTION_H
#define STONE_SEM_USINGRESOLUTION_H

namespace stone {
namespace syn {
class SyntaxFile;
} // namespace syn

namespace sem {
// TODO: ModuleResolution
void ResolveUsings(syn::SyntaxFile &syntaxFile);
} // namespace sem
} // namespace stone

#endif
