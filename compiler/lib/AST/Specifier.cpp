#include "stone/AST/Specifier.h"

using namespace stone;

static bool IsBasicTypeImpl(TypeSpecifierKind kind) {
  switch (kind) {
  case TypeSpecifierKind::Void:
  case TypeSpecifierKind::Any:
  case TypeSpecifierKind::Float:
  case TypeSpecifierKind::Float32:
  case TypeSpecifierKind::Float64:
  case TypeSpecifierKind::Int:
  case TypeSpecifierKind::Int8:
  case TypeSpecifierKind::Int16:
  case TypeSpecifierKind::Int32:
  case TypeSpecifierKind::Int64:
  case TypeSpecifierKind::UInt:
  case TypeSpecifierKind::UInt8:
  case TypeSpecifierKind::Byte:
  case TypeSpecifierKind::UInt16:
  case TypeSpecifierKind::UInt32:
  case TypeSpecifierKind::UInt64:
  case TypeSpecifierKind::Complex32:
  case TypeSpecifierKind::Complex64:
  case TypeSpecifierKind::Imaginary32:
  case TypeSpecifierKind::Imaginary64:
    return true;
  default:
    return false;
  }
}
static bool IsNominalTypeImpl(TypeSpecifierKind kind) {
  switch (kind) {
  case TypeSpecifierKind::Struct:
  case TypeSpecifierKind::Interface:
  case TypeSpecifierKind::Enum:
    return true;
  default:
    return false;
  }
}

static bool IsMiscTypeImpl(TypeSpecifierKind kind) {
  switch (kind) {
  case TypeSpecifierKind::Auto:
    return true;
  default:
    return false;
  }
}
bool TypeSpecifierCollector::IsBasicType() { return IsBasicTypeImpl(kind); }
bool TypeSpecifierCollector::IsNominalType() { return IsNominalTypeImpl(kind); }

void TypeSpecifierCollector::AddTypeSpecifierKind(TypeSpecifierKind inputKind,
                                                  SrcLoc inputLoc) {

  assert((IsBasicTypeImpl(inputKind) || IsNominalTypeImpl(inputKind) ||
          IsMiscTypeImpl(inputKind)) &&
         "Unknown type specifier");

  if (GetKind() == TypeSpecifierKind::None) {
    kind = inputKind;
    loc = inputLoc;
  } else {
    assert(false && "Can only have one 'type specifier'");
  }
}

void TypeSpecifierCollector::AddTypeNullabilityKind(TypeNullabilityKind kind) {}

void TypeSpecifierCollector::AddAuto(SrcLoc loc) {
  AddTypeSpecifierKind(TypeSpecifierKind::Auto, loc);
}
void TypeSpecifierCollector::AddBool(SrcLoc loc) {
  AddTypeSpecifierKind(TypeSpecifierKind::Bool, loc);
}

void TypeSpecifierCollector::AddChar(SrcLoc loc) {
  AddTypeSpecifierKind(TypeSpecifierKind::Char, loc);
}

void TypeSpecifierCollector::AddFloat(SrcLoc loc) {

  AddTypeSpecifierKind(TypeSpecifierKind::Bool, loc);
}
void TypeSpecifierCollector::AddFloat32(SrcLoc loc) {

  AddTypeSpecifierKind(TypeSpecifierKind::Float32, loc);
}

void TypeSpecifierCollector::AddFloat64(SrcLoc loc) {

  AddTypeSpecifierKind(TypeSpecifierKind::Float64, loc);
}
void TypeSpecifierCollector::AddEnum(SrcLoc loc) {

  AddTypeSpecifierKind(TypeSpecifierKind::Enum, loc);
}
void TypeSpecifierCollector::AddInterface(SrcLoc loc) {

  AddTypeSpecifierKind(TypeSpecifierKind::Interface, loc);
}
void TypeSpecifierCollector::AddStruct(SrcLoc loc) {

  AddTypeSpecifierKind(TypeSpecifierKind::Struct, loc);
}
void TypeSpecifierCollector::AddVoid(SrcLoc loc) {

  AddTypeSpecifierKind(TypeSpecifierKind::Void, loc);
}
void TypeSpecifierCollector::AddAny(SrcLoc loc) {

  AddTypeSpecifierKind(TypeSpecifierKind::Any, loc);
}
void TypeSpecifierCollector::AddInt(SrcLoc loc) {

  AddTypeSpecifierKind(TypeSpecifierKind::Int, loc);
}
void TypeSpecifierCollector::AddInt8(SrcLoc loc) {

  AddTypeSpecifierKind(TypeSpecifierKind::Int8, loc);
}
void TypeSpecifierCollector::AddInt16(SrcLoc loc) {

  AddTypeSpecifierKind(TypeSpecifierKind::Int16, loc);
}
void TypeSpecifierCollector::AddInt32(SrcLoc loc) {

  AddTypeSpecifierKind(TypeSpecifierKind::Int32, loc);
}
void TypeSpecifierCollector::AddInt64(SrcLoc loc) {

  AddTypeSpecifierKind(TypeSpecifierKind::Int64, loc);
}
void TypeSpecifierCollector::AddUInt(SrcLoc loc) {

  AddTypeSpecifierKind(TypeSpecifierKind::UInt, loc);
}
void TypeSpecifierCollector::AddUInt8(SrcLoc loc) {

  AddTypeSpecifierKind(TypeSpecifierKind::UInt, loc);
}
void TypeSpecifierCollector::AddByte(SrcLoc loc) {
  AddTypeSpecifierKind(TypeSpecifierKind::Byte, loc);
}
void TypeSpecifierCollector::AddUInt16(SrcLoc loc) {
  AddTypeSpecifierKind(TypeSpecifierKind::UInt16, loc);
}
void TypeSpecifierCollector::AddUInt32(SrcLoc loc) {

  AddTypeSpecifierKind(TypeSpecifierKind::UInt32, loc);
}
void TypeSpecifierCollector::AddUInt64(SrcLoc loc) {

  AddTypeSpecifierKind(TypeSpecifierKind::UInt64, loc);
}
void TypeSpecifierCollector::AddComplex32(SrcLoc loc) {

  AddTypeSpecifierKind(TypeSpecifierKind::Complex32, loc);
}
void TypeSpecifierCollector::AddComplex64(SrcLoc loc) {

  AddTypeSpecifierKind(TypeSpecifierKind::Complex64, loc);
}
void TypeSpecifierCollector::AddImaginary32(SrcLoc loc) {
  AddTypeSpecifierKind(TypeSpecifierKind::Imaginary32, loc);
}
void TypeSpecifierCollector::AddImaginary64(SrcLoc loc) {
  AddTypeSpecifierKind(TypeSpecifierKind::Imaginary64, loc);
}

void TypeSpecifierCollector::Apply() {}
