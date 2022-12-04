#ifndef STONE_SYNTAX_SPECIFIER_H
#define STONE_SYNTAX_SPECIFIER_H

#include "stone/Basic/LLVM.h"
#include "stone/Basic/STDTypeAlias.h"
#include "stone/Basic/SrcLoc.h"
#include "stone/Syntax/TypeAlignment.h"

#include "llvm/ADT/SmallVector.h"

namespace stone {
namespace syn {

enum class TypeSpecifierKind : uint8_t {
  None = 0,
  Auto,
  Void,
  Any,
  Bool,
  Float,
  Float32,
  Float64,
  Enum,
  Interface,
  Struct,
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
  Imaginary64,

};

/// TODO: Flag it
enum class TypeSpecifierChunkKind : UInt8 {
  Pointer,
  Reference,
  Array,
  Function,
  MemberPointer,
  Paren,
  Pipe,
};

enum class FunctionInlineSpecifierKind : UInt8 {
  None = 0,
  Inline,
  ForcedInline,
};

// TODO: All you need is Public and Local
enum class AccessLevel : UInt8 {
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
enum class ExprValueKind : UInt8 {
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

/// A further classification of the kind of object referenced by an
/// l-value or x-value.
enum class ExprObjectKind {
  /// An ordinary object is located at an address in memory.
  Ordinary,

  /// A bitfield object is a bitfield on a struct.
  BitField,

  /// A vector component is an element or range of elements on a vector.
  VectorComponent,

  /// A matrix component is a single element of a matrix.
  MatrixComponent
};

/// Storage classes.
/// These are legal on both functions and variables
enum class StorageSpecifierKind : UInt8 {
  None = 0,
  // TODO: You may not need extern
  Extern,
  Static,
  // Legal only on variables.
  Register
};

/// The storage duration for an object (per C++ [ctx.stc]).
enum class StorageDuration : UInt8 {
  FullExpression, ///< Full-expression storage duration (for temporaries).
  Automatic,      ///< Automatic storage duration (most local variables).
  Thread,         ///< Thread storage duration.
  Static,         ///< Static storage duration.
  Dynamic         ///< Dynamic storage duration.
};

/// Describes the nullability of a particular type.
enum class TypeNullabilityKind : UInt8 {
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
enum class TemplateSpecializationKind : UInt8 {
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

class TypeSpecifierCollector final {

  SrcLoc typeSpecifierLoc;
  TypeSpecifierKind typeSpecifierKind;

  // TODO: remove -- do not need this part.
  TypeNullabilityKind nullabilityKind;

public:
  TypeSpecifierCollector() : typeSpecifierKind(TypeSpecifierKind::None) {}

public:
  bool SetTypeSpecifierKind(TypeSpecifierKind kind, SrcLoc loc);

  bool HasAny() const { return typeSpecifierKind != TypeSpecifierKind::None; }
  bool NotHasAny() const {
    return typeSpecifierKind == TypeSpecifierKind::None;
  }

private:
  void AddTypeSpecifierKind(TypeSpecifierKind kind, SrcLoc loc);
  void AddTypeNullabilityKind(TypeNullabilityKind kind);

public:
  // == Basic Types ==//
  void AddAuto(SrcLoc loc);
  void AddAny(SrcLoc loc);
  void AddVoid(SrcLoc loc);
  void AddBool(SrcLoc loc);
  void AddFloat(SrcLoc loc);
  void AddFloat32(SrcLoc loc);
  void AddFloat64(SrcLoc loc);
  void AddInt(SrcLoc loc);
  void AddInt8(SrcLoc loc);
  void AddInt16(SrcLoc loc);
  void AddInt32(SrcLoc loc);
  void AddInt64(SrcLoc loc);
  void AddUInt(SrcLoc loc);
  void AddUInt8(SrcLoc loc);
  void AddByte(SrcLoc loc);
  void AddUInt16(SrcLoc loc);
  void AddUInt32(SrcLoc loc);
  void AddUInt64(SrcLoc loc);
  void AddComplex32(SrcLoc loc);
  void AddComplex64(SrcLoc loc);
  void AddImaginary32(SrcLoc loc);
  void AddImaginary64(SrcLoc loc);

  // == Nominal Types ==//
  void AddEnum(SrcLoc loc);
  void AddInterface(SrcLoc loc);
  void AddStruct(SrcLoc loc);

public:
  TypeSpecifierKind GetTypeSpecifierKind() { return typeSpecifierKind; }
  TypeNullabilityKind GetTypeNullabilityKind() { return nullabilityKind; }

public:
  bool IsBasicType();
  bool IsNominalType();
  bool IsEnum() { return typeSpecifierKind == TypeSpecifierKind::Enum; }
  bool IsStruct() { return typeSpecifierKind == TypeSpecifierKind::Struct; }
  bool IsInterface() {
    return typeSpecifierKind == TypeSpecifierKind::Interface;
  }
  bool IsAuto() { return typeSpecifierKind == TypeSpecifierKind::Auto; }
  SrcLoc GetLoc() { return typeSpecifierLoc; }

public:
  void AddNotNullable() {
    AddTypeNullabilityKind(TypeNullabilityKind::NotNullable);
  }
  void AddNullable() { AddTypeNullabilityKind(TypeNullabilityKind::Nullable); }

  void AddUnspecifiedNullable() {
    AddTypeNullabilityKind(TypeNullabilityKind::Unspecified);
  }

public:
  void Apply();
};

} // namespace syn
} // namespace stone
#endif
