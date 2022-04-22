#ifndef STONE_SYNTAX_SPECIFIER_H
#define STONE_SYNTAX_SPECIFIER_H

#include "stone/Basic/LLVM.h"
#include "llvm/ADT/SmallVector.h"

namespace stone {
namespace syn {

enum class TypeSpecifierKind : unsigned {
  None = 0,
  Auto,
  Bool,
  Float32,
  Float64,
  Interface,
  Struct,
  Void,
  Int,
  Int8,
  Int16,
  Int32,
  Int64,
  UInt,
  UInt8,
  Byte, // Alias for Uint8
  UInt16,
  UInt32,
  UInt64,
  Complex32,
  Complex64,

};

// TODO: All you need is Public and Local
enum class AccessLevel : uint8_t {
  None = 0,
  /// Limited to the scope
  Private,
  /// Accessible only within module
  Internal,
  /// Open outside of module with certain restrictions
  Public,
  /// Completely open and available
  Global,
};

/// The categorization of expression values, currently following the
enum class ExprValueType : uint8_t {
  /// An r-value expression (a pr-value in the C++11 taxonomy)
  /// produces a temporary value.
  RValue,

  /// An l-value expression is a reference to an object with
  /// independent storage.
  LValue,

  /// An x-value expression is a reference to an object with
  /// independent storage but which can be "moved", i.e.
  /// efficiently cannibalized for its resources.
  XValue
};

/// Storage classes.
/// These are legal on both functions and variables
enum class StorageKind : uint8_t {
  None = 0,
  Extern,
  Static,
  // These are only legal on variables.
  Auto,
  Register
};

/// The storage duration for an object (per C++ [ctx.stc]).
enum class StorageDuration : uint8_t {
  FullExpression, ///< Full-expression storage duration (for temporaries).
  Automatic,      ///< Automatic storage duration (most local variables).
  Thread,         ///< Thread storage duration.
  Static,         ///< Static storage duration.
  Dynamic         ///< Dynamic storage duration.
};

/// Describes the nullability of a particular type.
enum class TypeNullabilityKind : uint8_t {
  /// Values of this type can never be null.
  NotNullable = 0,
  /// Values of this type can be null.
  Nullable,
  /// Whether values of this type can be null is (explicitly)
  /// unspecified. This captures a (fairly rare) case where we
  /// can't conclude anything about the nullability of the type even
  /// though it has been considered.
  Unspecified
};

} // namespace syn
} // namespace stone
#endif
