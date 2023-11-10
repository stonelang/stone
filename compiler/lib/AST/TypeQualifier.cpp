#include "stone/AST/TypeQualifier.h"
#include "stone/AST/ASTContext.h"

using namespace stone;

TypeQualifierCollector::TypeQualifierCollector() {}

TypeQualifiers TypeQualifierCollector::ComputeTypeQaulifiers() {

  /// TODO: ok for now
  TypeQualifiers qualifiers;

  if (HasConst()) {
    qualifiers.AddConst();
  }
  if (HasPerm()) {
    qualifiers.AddPerm();
  }
  if (HasOwn()) {
    qualifiers.AddOwn();
  }
  if (HasMutable()) {
    qualifiers.AddMutable();
  }
  return qualifiers;
}

// Type TypeQualifierCollector::Apply(const stone::ASTContext &astContext,
//                                        const Type *typePtr) const {

//   Type qualType(typePtr);
//   if (GetConst().IsValid()) {
//     qualType.AddConst(GetConst());
//   }
//   return qualType;
// }
