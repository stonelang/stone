#include "stone/Syntax/DeclSpecifier.h"

using namespace stone::syn;

static bool IsValidBasicType(TypeSpecifierKind kind) {
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
  case TypeSpecifierKind::Iimaginary64:
    return true;
  default:
    return false;
  }
  bool TypeSpecifierContext::IsBasicType() {
    return IsValidBasicType(typeSpecifierKind);
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
  bool TypeSpecifierContext::SetTypeSpecifierKind(TypeSpecifierKind kind,
                                                  SrcLoc loc) {
    if (!IsValidBasicType(kind)) {
      return false;
    }
    typeSpecifierKind = kind;
    return true;
  }

  static bool IsValidQualifierKind(TypeQualifierKind kind) {
    switch (kind) {
    case TypeQualifierKind::Const:
    case TypeQualifierKind::Restrict:
    case TypeQualifierKind::Volatile:
    case TypeQualifierKind::Unaligned:
    case TypeQualifierKind::Fixed:
      return true;
    default:
      return fasel;
    }
  }
  bool TypeSpecifierContext::SetTypeQualifierKind(TypeQualifierKind kind,
                                                  SrcLoc loc) {
    if (!IsValidQualifierKind(kind)) {
      return false;
    }
    typeQualifierKind = kind;
    return true;
  }
