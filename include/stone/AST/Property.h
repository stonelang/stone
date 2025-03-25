#ifndef STONE_AST_PROPERTY_H
#define STONE_AST_PROPERTY_H

#include "stone/AST/ASTAllocation.h"
#include "stone/AST/Attribute.h"
#include "stone/AST/Identifier.h"
#include "stone/AST/TypeAlignment.h"
#include "stone/AST/Visibility.h"
#include "stone/Basic/SrcLoc.h"

#include "llvm/ADT/DenseMapInfo.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/TrailingObjects.h"
#include "llvm/Support/VersionTuple.h"

#include "stone/Basic/SrcLoc.h"
#include "llvm/ADT/BitVector.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/SmallVector.h"
#include <cassert>
#include <cstdint>
#include <vector>

namespace stone {

enum class PropertyKind : uint8_t {
  None = 0, // Default value for uninitialized or null properties
#define PROPERTY(id, parent) id,
#define LAST_PROPERTY(id) Last_Type = id,
#define PROPERTY_RANGE(Id, FirstId, LastId)                                    \
  First_##Id##Type = FirstId, Last_##Id##Type = LastId,
#include "stone/AST/PropertyNode.def"
};

class alignas(1 << PropertyAlignInBits) Property
    : public ASTAllocation<Property> {
  PropertyKind kind;
  SrcLoc loc;

public:
  Property(PropertyKind kind, SrcLoc loc) : kind(kind), loc(loc) {}

public:
  PropertyKind GetKind() { return kind; }
  SrcLoc GetLoc() { return loc; }
};

class DeclProperty : public Property {
public:
  DeclProperty(PropertyKind kind, SrcLoc loc) : Property(kind, loc) {}
};

class DeclModifier : public DeclProperty {

public:
  DeclModifier(PropertyKind kind, SrcLoc loc) : DeclProperty(kind, loc) {}
};

class StorageModifier : public DeclModifier {
public:
  StorageModifier(PropertyKind kind, SrcLoc loc) : DeclModifier(kind, loc) {}
};

class StaticModifier : public StorageModifier {

public:
  StaticModifier(SrcLoc loc) : StorageModifier(PropertyKind::Static, loc) {}
};

class ExternModifier : public StorageModifier {
public:
  ExternModifier(SrcLoc loc) : StorageModifier(PropertyKind::Extern, loc) {}
};

class AutoModifier : public StorageModifier {
public:
  AutoModifier(SrcLoc loc) : StorageModifier(PropertyKind::Auto, loc) {}
};

class VisibilityModifier : public DeclModifier {
  VisibilityLevel visibilityLevel;

public:
  VisibilityModifier(VisibilityLevel level, SrcLoc loc)
      : DeclModifier(PropertyKind::Visibility, loc), visibilityLevel(level) {}

public:
  VisibilityLevel GetVisibilityLevel() { return visibilityLevel; }
  void SetVisibilityLevel(VisibilityLevel level) { visibilityLevel = level; }
};

class DeclAttribute : public DeclProperty {
public:
  DeclAttribute(PropertyKind kind, SrcLoc loc) : DeclProperty(kind, loc) {}
};

class InlineAttribute : public DeclAttribute {
public:
  InlineAttribute(SrcLoc loc) : DeclAttribute(PropertyKind::Inline, loc) {}
};

class DeprecatedAttribute : public DeclAttribute {
public:
  DeprecatedAttribute(SrcLoc loc)
      : DeclAttribute(PropertyKind::Deprecated, loc) {}
};

class TypeProperty : public Property {
public:
  TypeProperty(PropertyKind kind, SrcLoc loc) : Property(kind, loc) {}
};

class TypeModifier : public TypeProperty {
public:
  TypeModifier(PropertyKind kind, SrcLoc loc) : TypeProperty(kind, loc) {}
};

class ConstModifier : public TypeModifier {
public:
  ConstModifier(SrcLoc loc) : TypeModifier(PropertyKind::Const, loc) {}
};
class PureModifier : public TypeModifier {
public:
  PureModifier(SrcLoc loc) : TypeModifier(PropertyKind::Pure, loc) {}
};
class StoneModifier : public TypeModifier {
public:
  StoneModifier(SrcLoc loc) : TypeModifier(PropertyKind::Stone, loc) {}
};
class VolatileModifier : public TypeModifier {
public:
  VolatileModifier(SrcLoc loc) : TypeModifier(PropertyKind::Volatile, loc) {}
};
class MutableModifier : public TypeModifier {
public:
  MutableModifier(SrcLoc loc) : TypeModifier(PropertyKind::Mutable, loc) {}
};

class TypeAttribute : public TypeProperty {
public:
  TypeAttribute(PropertyKind kind, SrcLoc loc) : TypeProperty(kind, loc) {}
};

template <typename PropertyType> class PropertyList {
  llvm::BitVector propertyMask;
  llvm::DenseMap<PropertyKind, PropertyType *> properties;

public:
  PropertyList()
      : propertyMask(static_cast<unsigned>(PropertyKind::Last_Type) + 1) {}

  // Overload the bool operator
  explicit operator bool() const { return !properties.empty(); }

public:
  void AddProperty(PropertyType *property) {
    properties[property->GetKind()] = property;
    propertyMask.set(static_cast<unsigned>(property->GetKind()));
  }
  bool HasProperty(PropertyKind kind) const {
    return propertyMask.test(static_cast<unsigned>(kind));
  }
  bool HasProperty(PropertyType *property) const {
    assert(property && "Cannot add null property!");
    return HasProperty(property->GetKind());
  }

  PropertyType *GetProperty(PropertyKind kind) const {
    auto it = properties.find(kind);
    return it != properties.end() ? it->second : nullptr;
  }
  bool IsEmpty() const { return properties.size() == 0; }
};

class DeclPropertyList : public PropertyList<DeclProperty> {
  ASTContext &AC;

public:
  explicit DeclPropertyList(ASTContext &AC) : AC(AC) {}

public:
  void AddPublic(SrcLoc loc) {
    AddProperty(new (AC) VisibilityModifier(VisibilityLevel::Public, loc));
  }
  void AddPrivate(SrcLoc loc) {
    AddProperty(new (AC) VisibilityModifier(VisibilityLevel::Private, loc));
  }
  void AddInternal(SrcLoc loc) {
    AddProperty(new (AC) VisibilityModifier(VisibilityLevel::Internal, loc));
  }
  void AddStatic(SrcLoc loc) { AddProperty(new (AC) StaticModifier(loc)); }
  void AddExtern(SrcLoc loc) { AddProperty(new (AC) ExternModifier(loc)); }
  // void AddInlineAttribute(SrcLoc loc) {
  //   GetDeclState()->AddDeclProperty(new (parser.GetASTContext())
  //                                       InlineAttribute(loc));
  // }
};

class TypePropertyList : public PropertyList<TypeProperty> {
  ASTContext &AC;

public:
  explicit TypePropertyList(ASTContext &AC) : AC(AC) {}

public:
  void AddConst(SrcLoc loc) { AddProperty(new (AC) ConstModifier(loc)); }
  void AddPure(SrcLoc loc) { AddProperty(new (AC) PureModifier(loc)); }
  void AddStone(SrcLoc loc) { AddProperty(new (AC) StoneModifier(loc)); }
  void AddVolatile(SrcLoc loc) { AddProperty(new (AC) VolatileModifier(loc)); }
  void AddMutable(SrcLoc loc) { AddProperty(new (AC) MutableModifier(loc)); }
};

} // namespace stone

namespace llvm {
template <> struct DenseMapInfo<stone::PropertyKind> {
  static inline stone::PropertyKind getEmptyKey() {
    return stone::PropertyKind::None; // Ensure this value is not used as a
                                      // valid key
  }

  static inline stone::PropertyKind getTombstoneKey() {
    return static_cast<stone::PropertyKind>(
        static_cast<uint8_t>(stone::PropertyKind::Last_Type) +
        1); // Value beyond valid range
  }

  static unsigned getHashValue(stone::PropertyKind kind) {
    return static_cast<unsigned>(kind);
  }

  static bool isEqual(stone::PropertyKind lhs, stone::PropertyKind rhs) {
    return lhs == rhs;
  }
};
} // namespace llvm
#endif
