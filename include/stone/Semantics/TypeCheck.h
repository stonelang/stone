#ifndef STONE_SEMANTICS_CHECK_H
#define STONE_SEMANTICS_CHECK_H

#include "llvm/ADT/ArrayRef.h"

namespace stone {
class TypeCheckerListener;

namespace syn {
class SyntaxFile;

} // namespace syn

namespace sema {
class TypeCheckerOptions;
/// Perform type checking
void TypeCheckSyntaxFile(syn::SyntaxFile &sf,
                         TypeCheckerOptions &typeCheckerOpts,
                         TypeCheckerListener *pipeline = nullptr);

/// Perform type checking
void TypeCheckModule(syn::SyntaxFile &sf, TypeCheckerOptions &typeCheckerOpts,
                     TypeCheckerListener *pipeline = nullptr);
} // namespace sema

} // namespace stone
#endif
