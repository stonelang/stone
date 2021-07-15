#ifndef STONE_COMPILE_CHECK_H
#define STONE_COMPILE_CHECK_H

#include "llvm/ADT/ArrayRef.h"

namespace stone {

namespace syn {
class Syntax;
class SyntaxFile;

} // namespace syn

namespace sema {
class CheckerPipeline;

/// Perform type checking
void CheckSyntaxFile(syn::SyntaxFile &sf, syn::Syntax &syntax,
                     CheckerPipeline *pipeline = nullptr);

/// Perform type checking
void CheckModule(syn::SyntaxFile &sf, syn::Syntax &syntax,
                 CheckerPipeline *pipeline = nullptr);
} // namespace sema

} // namespace stone
#endif
