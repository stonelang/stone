#ifndef STONE_AST_DECLSTATE_H
#define STONE_AST_DECLSTATE_H

#include "stone/AST/ASTAllocation.h"
#include "stone/AST/DeclName.h"

#include "stone/AST/Property.h"
#include "stone/AST/TypeAlignment.h"

#include "llvm/ADT/BitVector.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/Support/Allocator.h"

#include <cassert>

namespace stone {

class TypeState;

class alignas(1 << DeclAlignInBits) DeclState final : ASTAllocation<DeclState> {

  PropertyCollector<DeclProperty> properties;

  // The ASTContext associated with this DeclSate
  ASTContext *AC = nullptr;

  TypeState *typeState = nullptr;

  DeclName declName;

public:
  void SetTypeState(TypeState *TS) { typeState = TS; }
  TypeState *GetTypeState() { return typeState; }

  PropertyCollector<DeclProperty> &GetProperties() { return properties; }

  bool HasProperty(PropertyKind kind) const {
    return properties.HasProperty(kind);
  }
  void AddProperty(PropertyKind kind, DeclProperty *property) {
    properties.AddProperty(kind, property);
  }
  void SetDeclName(DeclName name) { declName = name; }
  DeclName GetDeclName() { return declName; }

private:
  // Direct comparison is disabled for states
  void operator==(DeclState D) const = delete;
  void operator!=(DeclState D) const = delete;
};

} // namespace stone
#endif
