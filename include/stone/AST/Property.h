#ifndef STONE_AST_PROPERTY_H
#define STONE_AST_PROPERTY_H

#include "stone/AST/ASTAllocation.h"
#include "stone/AST/AttributeKind.h"
#include "stone/AST/Identifier.h"
#include "stone/AST/TypeAlignment.h"
#include "stone/Basic/SrcLoc.h"

#include "llvm/ADT/DenseMapInfo.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/TrailingObjects.h"
#include "llvm/Support/VersionTuple.h"

namespace stone {

enum class PropertyKind : uint8_t {
  None = 0, // Default value for uninitialized or null properties
#define PROPERTY(id, parent) id,
#define LAST_PROPERTY(id) Last_Type = id,
#define PROPERTY_RANGE(Id, FirstId, LastId)                                    \
  First_##Id##Type = FirstId, Last_##Id##Type = LastId,
#include "stone/AST/PropertyNode.def"
};

// Define the enum class
enum class PropertyAvailability : uint8_t {
  None = 1 << 0,   // Default, unspecified availability
  Active = 1 << 1, // Modifier is active
  Ignore = 1 << 2, // Modifier is ignored
};

enum class PropertyScope : uint8_t {
  None = 0, // Default, unspecified scope
  Global,   // For global declarations
  Member,   // For class or struct members
  Param,    // For param members
  Local     // For local variables or parameters
};

class alignas(1 << PropertyAlignInBits) Property
    : public ASTAllocation<Property> {
  PropertyKind kind;
  SrcLoc loc;
  PropertyScope scope;
  PropertyAvailability availability;

public:
  Property(PropertyKind kind, SrcLoc loc) : kind(kind), loc(loc) {}

public:
  PropertyKind GetKind() { return kind; }
  SrcLoc GetLoc() { return loc; }

public:
  void SetScope(PropertyScope S) { scope = S; }
  PropertyScope GetScope() { return scope; }
  bool IsGlobal() const { return scope == PropertyScope::Global; }
  bool IsMember() const { return scope == PropertyScope::Member; }
  bool IsParam() const { return scope == PropertyScope::Param; }
  bool IsLocal() const { return scope == PropertyScope::Local; }

public:
  PropertyAvailability GetAvailability() const { return availability; }
  void SetAvailability(PropertyAvailability avail) { availability = avail; }
  // Check specific availability states
  bool IsActive() const { return availability == PropertyAvailability::Active; }
  bool IsIgnored() const {
    return availability == PropertyAvailability::Ignore;
  }

public:
  // bool IsModifier() const {
  //   return kind >= PropertyKind::First_ModifierType &&
  //          kind <= PropertyKind::Last_ModifierType;
  // }

  // bool IsAttribute() const;

  bool Matches(PropertyKind targetKind) const { return kind == targetKind; }
};

class DeclProperty : public Property {
public:
  DeclProperty(PropertyKind kind, SrcLoc loc) : Property(kind, loc) {}
};

class DeclModifier : public DeclProperty {
public:
  DeclModifier(PropertyKind kind, SrcLoc loc) : DeclProperty(kind, loc) {}

public:
  bool IsStorage() const;
  bool IsVisibility() const;
};

class StaticModifier : public DeclModifier {

public:
  StaticModifier(SrcLoc loc) : DeclModifier(PropertyKind::Static, loc) {}
};

class ExternModifier : public DeclModifier {
public:
  ExternModifier(SrcLoc loc) : DeclModifier(PropertyKind::Extern, loc) {}
};
class PublicModifier : public DeclModifier {
public:
  PublicModifier(SrcLoc loc) : DeclModifier(PropertyKind::Public, loc) {}
};
class ProtectedModifier : public DeclModifier {
public:
  ProtectedModifier(SrcLoc loc) : DeclModifier(PropertyKind::Protected, loc) {}
};
class PrivateModifier : public DeclModifier {
public:
  PrivateModifier(SrcLoc loc) : DeclModifier(PropertyKind::Private, loc) {}
};

class DeclAttribute : public DeclProperty {
public:
  DeclAttribute(PropertyKind kind, SrcLoc loc) : DeclProperty(kind, loc) {}
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

class TypeAttribute : public TypeProperty {
public:
  TypeAttribute(PropertyKind kind, SrcLoc loc) : TypeProperty(kind, loc) {}
};
class AlignAttribute : public TypeAttribute {
public:
  AlignAttribute(SrcLoc loc) : TypeAttribute(PropertyKind::Aligned, loc) {}
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