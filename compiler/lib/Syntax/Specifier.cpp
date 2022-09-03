#include "stone/Syntax/Specifier.h"

using namespace stone;
using namespace stone::syn;

static bool IsValidTypeSpecifierKind(TypeSpecifierKind kind) {
  switch (kind) {
  case TypeSpecifierKind::Auto:
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
bool TypeSpecifierContext::IsBasicType() {
  return IsValidTypeSpecifierKind(typeSpecifierKind);
}
bool TypeSpecifierContext::IsNominalType() {
  switch (typeSpecifierKind) {
  case TypeSpecifierKind::Struct:
  case TypeSpecifierKind::Interface:
  case TypeSpecifierKind::Enum:
    return true;
  default:
    return false;
  }
}
// bool TypeSpecifierContext::SetTypeSpecifierKind(TypeSpecifierKind kind,
//                                                 SrcLoc loc) {
//   if (!IsValidTypeSpecifierKind(kind)) {
//     return false;
//   }
//   typeSpecifierKind = kind;
//   return true;
// }

void TypeSpecifierContext::AddTypeSpecifierKind(TypeSpecifierKind kind,
                                                SrcLoc loc) {
  assert((typeSpecifierKind == TypeSpecifierKind::None) &&
         "can only have one 'type specifier type'");
  assert(IsValidTypeSpecifierKind(kind) && "alien specifier type");

  typeSpecifierKind = kind;
  typeSpecifierLoc = loc;
}
void TypeSpecifierContext::AddAuto(SrcLoc loc) {
  AddTypeSpecifierKind(TypeSpecifierKind::Auto, loc);
}
void TypeSpecifierContext::AddBool(SrcLoc loc) {
  AddTypeSpecifierKind(TypeSpecifierKind::Bool, loc);
}
void TypeSpecifierContext::AddFloat(SrcLoc loc) {

  AddTypeSpecifierKind(TypeSpecifierKind::Bool, loc);
}
void TypeSpecifierContext::AddFloat32(SrcLoc loc) {

  AddTypeSpecifierKind(TypeSpecifierKind::Float32, loc);
}

void TypeSpecifierContext::AddFloat64(SrcLoc loc) {

  AddTypeSpecifierKind(TypeSpecifierKind::Float64, loc);
}
void TypeSpecifierContext::AddEnum(SrcLoc loc) {

  AddTypeSpecifierKind(TypeSpecifierKind::Enum, loc);
}
void TypeSpecifierContext::AddInterface(SrcLoc loc) {

  AddTypeSpecifierKind(TypeSpecifierKind::Interface, loc);
}
void TypeSpecifierContext::AddStruct(SrcLoc loc) {

  AddTypeSpecifierKind(TypeSpecifierKind::Struct, loc);
}
void TypeSpecifierContext::AddVoid(SrcLoc loc) {

  AddTypeSpecifierKind(TypeSpecifierKind::Void, loc);
}
void TypeSpecifierContext::AddInt(SrcLoc loc) {

  AddTypeSpecifierKind(TypeSpecifierKind::Int, loc);
}
void TypeSpecifierContext::AddInt8(SrcLoc loc) {

  AddTypeSpecifierKind(TypeSpecifierKind::Int8, loc);
}
void TypeSpecifierContext::AddInt16(SrcLoc loc) {

  AddTypeSpecifierKind(TypeSpecifierKind::Int16, loc);
}
void TypeSpecifierContext::AddInt32(SrcLoc loc) {

  AddTypeSpecifierKind(TypeSpecifierKind::Int32, loc);
}
void TypeSpecifierContext::AddInt64(SrcLoc loc) {

  AddTypeSpecifierKind(TypeSpecifierKind::Int64, loc);
}
void TypeSpecifierContext::AddUInt(SrcLoc loc) {

  AddTypeSpecifierKind(TypeSpecifierKind::UInt, loc);
}
void TypeSpecifierContext::AddUInt8(SrcLoc loc) {

  AddTypeSpecifierKind(TypeSpecifierKind::UInt, loc);
}
void TypeSpecifierContext::AddByte(SrcLoc loc) {
  AddTypeSpecifierKind(TypeSpecifierKind::Byte, loc);
}
void TypeSpecifierContext::AddUInt16(SrcLoc loc) {
  AddTypeSpecifierKind(TypeSpecifierKind::UInt16, loc);
}
void TypeSpecifierContext::AddUInt32(SrcLoc loc) {

  AddTypeSpecifierKind(TypeSpecifierKind::UInt32, loc);
}
void TypeSpecifierContext::AddUInt64(SrcLoc loc) {

  AddTypeSpecifierKind(TypeSpecifierKind::UInt64, loc);
}
void TypeSpecifierContext::AddComplex32(SrcLoc loc) {

  AddTypeSpecifierKind(TypeSpecifierKind::Complex32, loc);
}
void TypeSpecifierContext::AddComplex64(SrcLoc loc) {

  AddTypeSpecifierKind(TypeSpecifierKind::Complex64, loc);
}
void TypeSpecifierContext::AddImaginary32(SrcLoc loc) {
  AddTypeSpecifierKind(TypeSpecifierKind::Imaginary32, loc);
}
void TypeSpecifierContext::AddImaginary64(SrcLoc loc) {
  AddTypeSpecifierKind(TypeSpecifierKind::Imaginary64, loc);
}
