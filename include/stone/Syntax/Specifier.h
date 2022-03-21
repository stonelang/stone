#ifndef STONE_SYNTAX_SPECIFIER_H
#define STONE_SYNTAX_SPECIFIER_H

#include "stone/Core/LLVM.h"
#include "llvm/ADT/SmallVector.h"

namespace stone {
namespace syn {

enum class BuiltinType {
  None,
  Auto,
  Bool,
  Float32,
  Float64,
  Interface,
  Struct,
  Void,
  Null,
  Int,
  Int8,
  Int16,
  Int32,
  Int64,
  UInt,
  Uint8,
  Byte, // Alias for Uint8
  Uint16,
  Uint32,
  Uint64,
  Complex32,
  Complex64,

};

// TODO: All you need is Public and Local
enum class AccessLevel : uint8_t {
  None,
  Internal, // TODO: Rename to Local
  Public,

  //  TODO: you may not need this -- public and internal given the correct
  //  context may Suffice. Ex:
  /// struct Particle { internal int size; } means size is only accessible
  /// within particle internal struct Planet{} means that Planet is only
  /// accessible within the module
  Private,
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
  None,
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
enum class NullabilityType : uint8_t {
  /// Values of this type can never be null.
  NonNull = 0,
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
