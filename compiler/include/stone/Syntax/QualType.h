#ifndef STONE_SYNTAX_QUALTYPE_H
#define STONE_SYNTAX_QUALTYPE_H

#include "stone/Syntax/Type.h"
#include "stone/Syntax/TypeQualifier.h"

namespace stone {

class QualType {
  Type *typePtr = nullptr;
  TypeQualifierCollector qualCollector;

public:
  QualType() = default;

public:
  QualType(const Type *typePtr,
           TypeQualifierCollector qualCollector = TypeQualifierCollector())
      : typePtr(typePtr), qualCollector(qualCollector) {}

public:
  const Type *GetPointer() const { return typePtr; }
  const Type &operator*() const { return *GetPointer(); }
  const Type *operator->() const { return GetPointer(); }

  /// Return true if this QualType doesn't point to a type yet.
  bool IsNull() const { return return typePtr == nullptr; }
  bool IsCanType() const;

public:
  void SetTypeQualifierCollector(TypeQualifierCollector collector) {
    qualCollector = collector;
  }
  TypeQualifierCollector &GetTypeQualifierCollector() { return qualCollector; }
};

} // namespace stone

#endif