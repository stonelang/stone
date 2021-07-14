#ifndef STONE_ANALYZE_CHECK_H
#define STONE_ANALYZE_CHECK_H

#include "llvm/ADT/ArrayRef.h"

namespace stone {
class CheckerPipeline;

namespace syn {
class Syntax;
class SyntaxFile;

} // namespace syn

/// Perform type checking
void CheckSyntaxFile(syn::SyntaxFile &sf, syn::Syntax &syntax,
                     CheckerPipeline *pipeline = nullptr);

/// Perform type checking
void CheckModule(syn::SyntaxFile &sf, syn::Syntax &syntax,
                 CheckerPipeline *pipeline = nullptr);

} // namespace stone
#endif
