#ifndef STONE_AST_MODIFIER_H
#define STONE_AST_MODIFIER_H

#include "stone/AST/ASTAllocation.h"
#include "stone/AST/Identifier.h"
#include "stone/AST/TypeAlignment.h"
#include "stone/Basic/Memory.h"
#include "stone/Basic/SrcLoc.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/TrailingObjects.h"
#include "llvm/Support/VersionTuple.h"

#include <cstdint>

namespace stone {

enum class ModifierKind : uint8_t {
#define MODIFIER(ID, PARENT) ID,
#define LAST_MODIFIER(ID) Last_Modifier = ID,
#include "ModifierKind.def"
};

// Define the enum class
enum class ModifierAvailability : uint8_t {
  None = 0,        // Default, unspecified availability
  Active = 1 << 0, // Modifier is active
  Ignore = 1 << 1, // Modifier is ignored
};

// Enable bitwise operations for the enum class
inline ModifierAvailability operator|(ModifierAvailability lhs,
                                      ModifierAvailability rhs) {
  return static_cast<ModifierAvailability>(static_cast<uint8_t>(lhs) |
                                           static_cast<uint8_t>(rhs));
}

inline ModifierAvailability operator&(ModifierAvailability lhs,
                                      ModifierAvailability rhs) {
  return static_cast<ModifierAvailability>(static_cast<uint8_t>(lhs) &
                                           static_cast<uint8_t>(rhs));
}

inline ModifierAvailability operator^(ModifierAvailability lhs,
                                      ModifierAvailability rhs) {
  return static_cast<ModifierAvailability>(static_cast<uint8_t>(lhs) ^
                                           static_cast<uint8_t>(rhs));
}

inline ModifierAvailability operator~(ModifierAvailability availability) {
  return static_cast<ModifierAvailability>(~static_cast<uint8_t>(availability));
}

inline ModifierAvailability &operator|=(ModifierAvailability &lhs,
                                        ModifierAvailability rhs) {
  lhs = lhs | rhs;
  return lhs;
}

inline ModifierAvailability &operator&=(ModifierAvailability &lhs,
                                        ModifierAvailability rhs) {
  lhs = lhs & rhs;
  return lhs;
}

inline ModifierAvailability &operator^=(ModifierAvailability &lhs,
                                        ModifierAvailability rhs) {
  lhs = lhs ^ rhs;
  return lhs;
}
enum class ModifierScope : uint8_t {
  None = 0, // Default, unspecified scope
  Global,   // For global declarations
  Member,   // For class or struct members
  Param,    // For param members
  Local     // For local variables or parameters
};

class alignas(1 << ModifierAlignInBits) Modifier
    : public ASTAllocation<Modifier> {
  ModifierKind Kind;
  ModifierScope scope;
  SrcLoc loc;
  ModifierAvailability availability = ModifierAvailability::None;

public:
  Modifier(ModifierKind Kind) : Kind(Kind) {}

public:
  ModifierKind GetKind() { return Kind; }
  ModifierScope GetScope() { return scope; }
  void SetScope(ModifierScope S) { scope = S; }

  void SetLoc(SrcLoc L) { loc = L; }
  SrcLoc GetLoc() { return loc; }

public:
  // Add an availability state
  void AddAvailability(ModifierAvailability availabilityState) {
    availability |= availabilityState;
  }

  // Remove an availability state
  void RemoveAvailability(ModifierAvailability availabilityState) {
    availability &= ~availabilityState;
  }

  // Check if an availability state is set
  bool CheckAvailability(ModifierAvailability availabilityState) const {
    return static_cast<bool>(availability & availabilityState);
  }
};

class DeclModifier : public Modifier {
  DeclModifier *Next = nullptr;

public:
  DeclModifier(ModifierKind Kind) : Modifier(Kind) {}
};

class StorageDeclModifier : public DeclModifier {

public:
  StorageDeclModifier(ModifierKind Kind) : DeclModifier(Kind) {}
};

class StaticDeclModifier : public StorageDeclModifier {

public:
  StaticDeclModifier() : StorageDeclModifier(ModifierKind::Static) {}
};

class ExternDeclModifier : public StorageDeclModifier {
public:
  ExternDeclModifier() : StorageDeclModifier(ModifierKind::Extern) {}
};

class VisibilityDeclModifier : public DeclModifier {
public:
  VisibilityDeclModifier(ModifierKind Kind) : DeclModifier(Kind) {}
};

class PublicDeclModifier : public VisibilityDeclModifier {
public:
  PublicDeclModifier() : VisibilityDeclModifier(ModifierKind::Public) {}
};
class ProtectedDeclModifier : public VisibilityDeclModifier {
public:
  ProtectedDeclModifier() : VisibilityDeclModifier(ModifierKind::Protected) {}
};
class PrivateDeclModifier : public VisibilityDeclModifier {
public:
  PrivateDeclModifier() : VisibilityDeclModifier(ModifierKind::Protected) {}
};

/// Attributes that may be applied to declarations.
class DeclModifierList {
  /// Linked list of declaration attributes.
  DeclModifier *modifiers;
};

class TypeModifier : public Modifier {
  TypeModifier *Next = nullptr;

public:
  TypeModifier(ModifierKind Kind) : Modifier(Kind) {}
};

// class MutableTypeModifier : public TypeModifier { };

class VolatileTypeModifier : public TypeModifier {

public:
  VolatileTypeModifier() : TypeModifier(ModifierKind::Volatile) {}
};
class ConstTypeModifier : public TypeModifier {

public:
  ConstTypeModifier() : TypeModifier(ModifierKind::Const) {}
};
class PureTypeModifier : public TypeModifier {

public:
  PureTypeModifier() : TypeModifier(ModifierKind::Pure) {}
};
class StoneTypeModifier : public TypeModifier {

public:
  StoneTypeModifier() : TypeModifier(ModifierKind::Stone) {}
};

/// Attributes that may be applied to declarations.
class TypeModifierList {
  /// Linked list of declaration attributes.
  TypeModifier *modifiers;
};

} // namespace stone

#endif