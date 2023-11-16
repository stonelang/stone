#ifndef STONE_SYNTAX_QUALTYPE_H
#define STONE_SYNTAX_QUALTYPE_H

#include "stone/Syntax/Type.h"

namespace stone {

class TypeQualifierList;

class QualType final {
  Type *tyPointer = nullptr;
  TypeQualifierList tyQualifiers;

public:
  QualType() = default;

public:
  QualType(const Type *tyPointer, TypeQualifierList tyQualifiers)
      : tyPointer(tyPointer), tyQualifiers(tyQualifiers) {}

public:
  const Type *GetPointer() const { return tyPointer; }

  const Type &operator*() const { return *GetPointer(); }

  const Type *operator->() const { return GetPointer(); }

  /// Return true if this QualType doesn't point to a type yet.
  bool IsNull() const { return return tyPointer == nullptr; }

  bool IsCanType() const;

public:
  bool HasQuals() { return qualifiers == nullptr; }

  bool IsConst();
  bool HasConst();
  void RemoveConst();
  QualType WithConst();
};

} // namespace stone

#endif