#ifndef STONE_AST_SPECIFIER_H
#define STONE_AST_SPECIFIER_H

#include "stone/Basic/LLVM.h"
#include "stone/Basic/STDAlias.h"
#include "stone/Basic/SrcLoc.h"
#include "stone/AST/TypeAlignment.h"

#include "llvm/ADT/SmallVector.h"

namespace stone {
namespace syn {

enum class TypeSpecifierKind : uint8_t {
  None = 0,
  Auto,
  Void,
  Any,
  Bool,
  Char,
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

enum class FunctionInlineSpecifierKind : UInt8 {
  None = 0,
  Inline,
  ForcedInline,
};

enum class FunctionCallingConvention : UInt8 { Stone, C };

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

  SrcLoc loc;
  TypeSpecifierKind kind;

  // TODO: remove -- do not need this part.
  TypeNullabilityKind nullabilityKind;

public:
  TypeSpecifierCollector() : kind(TypeSpecifierKind::None) {}

public:
  bool SetTypeSpecifierKind(TypeSpecifierKind kind, SrcLoc inputLoc);

  bool HasAny() const { return kind != TypeSpecifierKind::None; }
  bool NotHasAny() const { return kind == TypeSpecifierKind::None; }

private:
  void AddTypeSpecifierKind(TypeSpecifierKind kind, SrcLoc inputLoc);
  void AddTypeNullabilityKind(TypeNullabilityKind kind);

public:
  // == Basic Types ==//
  void AddAuto(SrcLoc inputLoc);
  bool IsAuto() const {
    return (loc.isValid() && (kind == TypeSpecifierKind::Auto));
  }
  void AddAny(SrcLoc inputLoc);
  bool IsAny() const {
    return (loc.isValid() && (kind == TypeSpecifierKind::Any));
  }

  void AddVoid(SrcLoc inputLoc);
  bool IsVoid() const {
    return (loc.isValid() && (kind == TypeSpecifierKind::Void));
  }
  void AddBool(SrcLoc inputLoc);
  bool IsBool() const {
    return (loc.isValid() && (kind == TypeSpecifierKind::Bool));
  }

  void AddChar(SrcLoc inputLoc);
  bool IsChar() const {
    return (loc.isValid() && (kind == TypeSpecifierKind::Char));
  }

public:
  void AddFloat(SrcLoc inputLoc);
  bool IsFloat() const {
    return (loc.isValid() && (kind == TypeSpecifierKind::Float));
  }
  void AddFloat32(SrcLoc inputLoc);
  bool IsFloat32() const {
    return (loc.isValid() && (kind == TypeSpecifierKind::Float32));
  }
  void AddFloat64(SrcLoc inputLoc);
  bool IsFloat64() const {
    return (loc.isValid() && (kind == TypeSpecifierKind::Float64));
  }

public:
  void AddInt(SrcLoc inputLoc);
  bool IsInt() const {
    return (loc.isValid() && (kind == TypeSpecifierKind::Int));
  }
  void AddInt8(SrcLoc inputLoc);
  bool IsInt8() const {
    return (loc.isValid() && (kind == TypeSpecifierKind::Int8));
  }
  void AddInt16(SrcLoc inputLoc);
  bool IsInt16() const {
    return (loc.isValid() && (kind == TypeSpecifierKind::Int16));
  }
  void AddInt32(SrcLoc inputLoc);
  bool IsInt32() const {
    return (loc.isValid() && (kind == TypeSpecifierKind::Int32));
  }
  void AddInt64(SrcLoc inputLoc);
  bool IsInt64() const {
    return (loc.isValid() && (kind == TypeSpecifierKind::Int64));
  }

public:
  void AddUInt(SrcLoc inputLoc);
  void AddUInt8(SrcLoc inputLoc);
  void AddByte(SrcLoc inputLoc);
  void AddUInt16(SrcLoc inputLoc);
  void AddUInt32(SrcLoc inputLoc);
  void AddUInt64(SrcLoc inputLoc);

public:
  void AddComplex32(SrcLoc inputLoc);
  void AddComplex64(SrcLoc inputLoc);
  void AddImaginary32(SrcLoc inputLoc);
  void AddImaginary64(SrcLoc inputLoc);

  // == Nominal Types ==//
  void AddEnum(SrcLoc inputLoc);
  void AddInterface(SrcLoc inputLoc);
  void AddStruct(SrcLoc inputLoc);

public:
  TypeSpecifierKind GetKind() { return kind; }
  TypeNullabilityKind GetNullabilityKind() { return nullabilityKind; }

public:
  bool IsBasicType();
  bool IsNominalType();
  bool IsEnum() { return kind == TypeSpecifierKind::Enum; }
  bool IsStruct() { return kind == TypeSpecifierKind::Struct; }
  bool IsInterface() { return kind == TypeSpecifierKind::Interface; }
  bool IsAuto() { return kind == TypeSpecifierKind::Auto; }

public:
  void AddNotNullable() {
    AddTypeNullabilityKind(TypeNullabilityKind::NotNullable);
  }
  void AddNullable() { AddTypeNullabilityKind(TypeNullabilityKind::Nullable); }

  void AddUnspecifiedNullable() {
    AddTypeNullabilityKind(TypeNullabilityKind::Unspecified);
  }

  SrcLoc GetLoc() { return loc; }

public:
  void Apply();
};

} // namespace syn
} // namespace stone
#endif
