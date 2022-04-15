#ifndef STONE_COMPILE_USERESOLUTION_H
#define STONE_COMPILE_USERESOLUTION_H

namespace stone {
namespace syn {
class SyntaxFile;
} // namespace syn

namespace sem {
// TODO: ModuleResolution
void ResolveUsings(syn::SyntaxFile &syntaxFile);
} // namespace types
} // namespace stone

#endif
