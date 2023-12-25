#ifndef STONE_SYNTAX_STORAGE_SPECIFIER_H
#define STONE_SYNTAX_STORAGE_SPECIFIER_H

#include "stone/Basic/OptionSet.h"
#include "stone/Syntax/Attribute.h"

#include "llvm/ADT/ArrayRef.h"

namespace stone {

/// The storage duration for an object (per C++ [ctx.stc]).
enum class StorageDurationKind : UInt8 {
  None = 0,
  FullExpression, ///< Full-expression storage duration (for temporaries).
  Automatic,      ///< Automatic storage duration (most local variables).
  Thread,         ///< Thread storage duration.
  Static,         ///< Static storage duration.
  Dynamic         ///< Dynamic storage duration.
};

/// Storage classes.
/// These are legal on both functions and variables
enum class StorageSpecifierKind : UInt8 {
  None = 0, ///< TODO: You may not need extern
  Extern,   ///< functions
  Static,   ///< Variables, and Types.
  Register  ///< Legal only on variables.
};

} // namespace stone
#endif