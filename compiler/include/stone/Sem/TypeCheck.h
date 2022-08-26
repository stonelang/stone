#ifndef STONE_SEM_TYPECHECK_H
#define STONE_SEM_TYPECHECK_H

#include "llvm/ADT/ArrayRef.h"

namespace stone {
class TypeCheckerListener;

namespace syn {
class SyntaxFile;
class Module;

} // namespace syn
namespace sem {
class TypeCheckerOptions;
/// Perform type checking
void TypeCheck(syn::SyntaxFile &sf, TypeCheckerOptions &typeCheckerOpts,
               TypeCheckerListener *pipeline = nullptr);

/// Perform type checking
void TypeCheck(syn::Module &sf, TypeCheckerOptions &typeCheckerOpts,
               TypeCheckerListener *pipeline = nullptr);
} // namespace sem

} // namespace stone
#endif
