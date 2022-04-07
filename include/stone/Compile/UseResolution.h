#ifndef STONE_COMPILE_USERESOLUTION_H
#define STONE_COMPILE_USERESOLUTION_H

namespace stone {
namespace syn {
class SyntaxFile;
} // namespace syn

namespace types {
void ResolveUseDeclarations(syn::SyntaxFile &syntaxFile);
}
} // namespace stone

#endif
