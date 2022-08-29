#ifndef STONE_SYNTAX_SPECIFIER_H
#define STONE_SYNTAX_SPECIFIER_H

#include "stone/Basic/LLVM.h"
#include "llvm/ADT/SmallVector.h"

namespace stone {
namespace syn {

enum class TypeSpecifierKind : uint8_t {
  None = 0,
  Auto,
  Bool,
  Float,
  Float32,
  Float64,
  Enum,
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
  Imaginary32,
  Iimaginary64,

};
enum class FunctionSpecifierKind : uint8_t {
  None,
  Inline,
  ForcedInline,
  NoReturn,
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

  None = 0,
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
enum class StorageSpecifierKind : uint8_t {
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

/// Describes the kind of template specialization that a
/// particular template specialization declaration represents.
enum class TemplateSpecializationKind : uint8_t {
  /// This template specialization was formed from a template-id but
  /// has not yet been declared, defined, or instantiated.
  Undeclared = 0,
  /// This template specialization was implicitly instantiated from a
  /// template.
  ImplicitInstantiation,
  /// This template specialization was declared or defined by an
  /// explicit specialization
  ExplicitSpecialization,
  /// This template specialization was instantiated from a template
  /// due to an explicit instantiation declaration request
  ExplicitInstantiationDeclaration,
  /// This template specialization was instantiated from a template
  /// due to an explicit instantiation definition request
  ExplicitInstantiationDefinition
};

} // namespace syn
} // namespace stone
#endif
