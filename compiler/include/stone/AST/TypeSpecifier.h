#ifndef STONE_SYNTAX_SPECIFIER_H
#define STONE_SYNTAX_SPECIFIER_H

#include "stone/Basic/LLVM.h"
#include "stone/Basic/STDAlias.h"
#include "stone/Basic/SrcLoc.h"
#include "stone/AST/Types.h"

#include "llvm/ADT/SmallVector.h"

namespace stone {

enum class TypeSpecifierKind : uint8_t {
  None = 0,
#define TYPE_SPECIFIER(N) N,
#include "stone/AST/TypeSpecifierKind.def"
  MAX
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

// TODO:
//  class TypeSpecifierMemoryChunk {
//  };

class TypeSpecifierCollector final {

  SrcLoc loc;
  Type ty;
  TypeSpecifierKind specifierKind;
  TypeNullabilityKind nullabilityKind;

public:
  // llvm::SmallVector<TypeSpecifierMemoryChunk, 8> memAllocs;

public:
  TypeSpecifierCollector()
      : specifierKind(TypeSpecifierKind::None),
        nullabilityKind(TypeNullabilityKind::NotNullable) {}

public:
  void SetType(Type inputType) { ty = inputType; }
  Type GetType() { return ty; }

private:
  void AddTypeSpecifierKind(TypeSpecifierKind kind, SrcLoc inputLoc);
  void AddTypeNullabilityKind(TypeNullabilityKind kind);

public:
  bool SetTypeSpecifierKind(TypeSpecifierKind kind, SrcLoc inputLoc);
  TypeSpecifierKind GetSpecifierKind() { return specifierKind; }

  bool HasAny() const { return specifierKind != TypeSpecifierKind::None; }
  bool HasNone() const { return specifierKind == TypeSpecifierKind::None; }

public:
  void AddValueMemoryChunk();
  void AddPointerMemoryChunk(SrcLoc loc);

public:
  TypeNullabilityKind GetNullabilityKind() { return nullabilityKind; }
  void AddNotNullable() {
    AddTypeNullabilityKind(TypeNullabilityKind::NotNullable);
  }
  void AddNullable() { AddTypeNullabilityKind(TypeNullabilityKind::Nullable); }

  void AddUnspecifiedNullable() {
    AddTypeNullabilityKind(TypeNullabilityKind::Unspecified);
  }

public:
  // == Basic Types ==//
  bool IsBasicType();

  void AddAuto(SrcLoc inputLoc);
  bool IsAuto() const {
    return (loc.isValid() && (specifierKind == TypeSpecifierKind::Auto));
  }

  void AddAny(SrcLoc inputLoc);
  bool IsAny() const {
    return (loc.isValid() && (specifierKind == TypeSpecifierKind::Any));
  }

  void AddVoid(SrcLoc inputLoc);
  bool IsVoid() const {
    return (loc.isValid() && (specifierKind == TypeSpecifierKind::Void));
  }
  void AddBool(SrcLoc inputLoc);
  bool IsBool() const {
    return (loc.isValid() && (specifierKind == TypeSpecifierKind::Bool));
  }

  void AddChar(SrcLoc inputLoc);
  bool IsChar() const {
    return (loc.isValid() && (specifierKind == TypeSpecifierKind::Char));
  }

public:
  void AddFloat(SrcLoc inputLoc);
  bool IsFloat() const {
    return (loc.isValid() && (specifierKind == TypeSpecifierKind::Float));
  }
  void AddFloat32(SrcLoc inputLoc);
  bool IsFloat32() const {
    return (loc.isValid() && (specifierKind == TypeSpecifierKind::Float32));
  }
  void AddFloat64(SrcLoc inputLoc);
  bool IsFloat64() const {
    return (loc.isValid() && (specifierKind == TypeSpecifierKind::Float64));
  }

public:
  void AddInt(SrcLoc inputLoc);
  bool IsInt() const {
    return (loc.isValid() && (specifierKind == TypeSpecifierKind::Int));
  }
  void AddInt8(SrcLoc inputLoc);
  bool IsInt8() const {
    return (loc.isValid() && (specifierKind == TypeSpecifierKind::Int8));
  }
  void AddInt16(SrcLoc inputLoc);
  bool IsInt16() const {
    return (loc.isValid() && (specifierKind == TypeSpecifierKind::Int16));
  }
  void AddInt32(SrcLoc inputLoc);
  bool IsInt32() const {
    return (loc.isValid() && (specifierKind == TypeSpecifierKind::Int32));
  }
  void AddInt64(SrcLoc inputLoc);
  bool IsInt64() const {
    return (loc.isValid() && (specifierKind == TypeSpecifierKind::Int64));
  }

  void AddString(SrcLoc inputLoc);
  bool IsString() const {
    return (loc.isValid() && (specifierKind == TypeSpecifierKind::String));
  }

public:
  void AddUInt(SrcLoc inputLoc);
  bool IsUInt() const {
    return (loc.isValid() && (specifierKind == TypeSpecifierKind::UInt));
  }
  void AddUInt8(SrcLoc inputLoc);
  bool IsUInt8() const {
    return (loc.isValid() && (specifierKind == TypeSpecifierKind::UInt8));
  }
  void AddByte(SrcLoc inputLoc);
  bool IsByte() const {
    return (loc.isValid() && (specifierKind == TypeSpecifierKind::Byte));
  }

  void AddUInt16(SrcLoc inputLoc);
  bool IsUInt16() const {
    return (loc.isValid() && (specifierKind == TypeSpecifierKind::UInt16));
  }
  void AddUInt32(SrcLoc inputLoc);
  bool IsUInt32() const {
    return (loc.isValid() && (specifierKind == TypeSpecifierKind::UInt32));
  }
  void AddUInt64(SrcLoc inputLoc);
  bool IsUInt64() const {
    return (loc.isValid() && (specifierKind == TypeSpecifierKind::UInt64));
  }

public:
  void AddComplex32(SrcLoc inputLoc);
  bool IsComplex32() const {
    return (loc.isValid() && (specifierKind == TypeSpecifierKind::Complex32));
  }
  void AddComplex64(SrcLoc inputLoc);
  bool IsComplex64() const {
    return (loc.isValid() && (specifierKind == TypeSpecifierKind::Complex64));
  }
  void AddImaginary32(SrcLoc inputLoc);
  bool IsImaginary32() const {
    return (loc.isValid() && (specifierKind == TypeSpecifierKind::Imaginary32));
  }

  void AddImaginary64(SrcLoc inputLoc);
  bool IsImaginary64() const {
    return (loc.isValid() && (specifierKind == TypeSpecifierKind::Imaginary64));
  }

public:
  // == Nominal Types ==//
  bool IsNominalType();

  void AddEnum(SrcLoc inputLoc);
  bool IsEnum() {
    return (loc.isValid() && specifierKind == TypeSpecifierKind::Enum);
  }

  void AddInterface(SrcLoc inputLoc);
  bool IsInterface() {
    return (loc.isValid() && specifierKind == TypeSpecifierKind::Interface);
  }

  void AddStruct(SrcLoc inputLoc);
  bool IsStruct() {
    return (loc.isValid() && specifierKind == TypeSpecifierKind::Struct);
  }

  SrcLoc GetLoc() { return loc; }

public:
  void Clear() {
    loc = SrcLoc();
    specifierKind = TypeSpecifierKind::None;
  }

public:
  void Apply();
};

} // namespace stone
#endif
