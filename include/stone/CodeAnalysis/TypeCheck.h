#ifndef STONE_CODEANALYSIS_CHECK_H
#define STONE_CODEANALYSIS_CHECK_H

#include "llvm/ADT/ArrayRef.h"

namespace stone {
class CheckerPipeline;

namespace syn {
class Syntax;
class SyntaxFile;

} // namespace syn

namespace sema {
/// Perform type checking
void TypeCheckSyntaxFile(syn::SyntaxFile &sf, syn::Syntax &syntax,
                     TypeCheckerPipeline *pipeline = nullptr);

/// Perform type checking
void TypeCheckModule(syn::SyntaxFile &sf, syn::Syntax &syntax,
                 TypeCheckerPipeline *pipeline = nullptr);
} // namespace sema

} // namespace stone
#endif
