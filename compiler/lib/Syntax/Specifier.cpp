#include "stone/Syntax/Specifier.h"

using namespace stone;
using namespace stone::syn;

bool IsValidTypeSpecifierKind(TypeSpecifierKind kind) {
  switch (kind) {
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
bool TypeSpecifierCollector::IsBasicType() {
  return IsValidTypeSpecifierKind(typeSpecifierKind);
}
bool TypeSpecifierCollector::IsNominalType() {
  switch (typeSpecifierKind) {
  case TypeSpecifierKind::Struct:
  case TypeSpecifierKind::Interface:
  case TypeSpecifierKind::Enum:
    return true;
  default:
    return false;
  }
}
// bool TypeSpecifierCollector::SetTypeSpecifierKind(TypeSpecifierKind kind,
//                                                 SrcLoc loc) {
//   if (!IsValidTypeSpecifierKind(kind)) {
//     return false;
//   }
//   typeSpecifierKind = kind;
//   return true;
// }

void TypeSpecifierCollector::AddTypeSpecifierKind(TypeSpecifierKind kind,
                                                SrcLoc loc) {
  assert((typeSpecifierKind == TypeSpecifierKind::None) &&
         "can only have one 'type specifier type'");
  assert(IsValidTypeSpecifierKind(kind) && "alien specifier type");

  typeSpecifierKind = kind;
  typeSpecifierLoc = loc;
}
void TypeSpecifierCollector::AddAuto(SrcLoc loc) {
  AddTypeSpecifierKind(TypeSpecifierKind::Auto, loc);
}
void TypeSpecifierCollector::AddBool(SrcLoc loc) {
  AddTypeSpecifierKind(TypeSpecifierKind::Bool, loc);
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
