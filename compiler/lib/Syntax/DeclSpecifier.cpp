#include "stone/Syntax/DeclSpecifier.h"

using namespace stone::syn;

bool TypeSpecifierContext::IsBasicType() {
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
  case TypeSpecifierKind::Iimaginary64:
    return true;
  default:
    return false;
  }
}
bool TypeSpecifierContext::IsNominalType() {
  switch (kind) {
  case TypeSpecifierKind::Struct:
  case TypeSpecifierKind::Interface:
  case TypeSpecifierKind::Enum:
    return true;
  default:
    return false;
  }
}