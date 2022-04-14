#ifndef STONE_CHECK_TYPECHECK_H
#define STONE_CHECK_TYPECHECK_H

#include "llvm/ADT/ArrayRef.h"

namespace stone {
class TypeCheckerListener;

namespace syn {
class SyntaxFile;
class Module; 

} // namespace syn
namespace types {
class TypeCheckerOptions;
/// Perform type checking
void TypeCheck(syn::SyntaxFile &sf, TypeCheckerOptions &typeCheckerOpts,
               TypeCheckerListener *pipeline = nullptr);

/// Perform type checking
void TypeCheck(syn::Module &sf, TypeCheckerOptions &typeCheckerOpts,
               TypeCheckerListener *pipeline = nullptr);
} // namespace types

} // namespace stone
#endif
