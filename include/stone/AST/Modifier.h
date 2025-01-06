#ifndef STONE_AST_MODIFIER_H
#define STONE_AST_MODIFIER_H

#include "stone/AST/Identifier.h"
#include "stone/AST/ModifierKind.h"
#include "stone/AST/TypeAlignment.h"
#include "stone/Basic/Memory.h"
#include "stone/Basic/SrcLoc.h"
#include "stone/AST/ASTAllocation.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/TrailingObjects.h"
#include "llvm/Support/VersionTuple.h"

namespace stone {

class alignas(1 << ModifierAlignInBits) Modifier
    : public ASTAllocation<Modifier> {};

enum class DeclModifierScope : uint8_t {
  None = 0, // Default, unspecified scope
  Global,   // For global declarations
  Member,   // For class or struct members
  Local     // For local variables or parameters
};

class DeclModifier : public Modifier {
  DeclModifier *Next = nullptr;
  DeclModifierScope scope = DeclModifierScope::None;
};

class StorageDeclModifier : public DeclModifier {};
class StaticDeclModifier : public StorageDeclModifier {};
class ExternDeclModifier : public StorageDeclModifier {};

class VisibilityDeclModifier : public DeclModifier {};
class PublicDeclModifier : public VisibilityDeclModifier {};
class ProtectedDeclModifier : public VisibilityDeclModifier {};
class PrivateDeclModifier : public VisibilityDeclModifier {};

/// Attributes that may be applied to declarations.
class DeclModifierList {
  /// Linked list of declaration attributes.
  DeclModifier *modifiers;
};

enum class TypeModifierScope : uint8_t { None = 0, Global, Parameter, Local };

class TypeModifier : public Modifier {
  TypeModifier *Next = nullptr;
  TypeModifierScope scope = TypeModifierScope::None;

public:
  TypeModifier(TypeModifierScope scope) : scope(scope) {}
};

// class MutableTypeModifier : public TypeModifier {

// };
// class VolatileTypeModifier : public TypeModifier {};
class ConstTypeModifier : public TypeModifier {

public:
  ConstTypeModifier(TypeModifierScope scope) : TypeModifier(scope) {}
};
// class PureTypeModifier : public TypeModifier {};
// class StoneTypeModifier : public TypeModifier {};

class AccessTypeModifier : public TypeModifier {
public:
  AccessTypeModifier(TypeModifierScope scope) : TypeModifier(scope) {}
};
class PtrTypeModifier : public AccessTypeModifier {
public:
  PtrTypeModifier(TypeModifierScope scope) : AccessTypeModifier(scope) {}
};
// class OwnTypeModifier : public AccessTypeModifier {};
//  class MoveTypeModifier : public AccessTypeModifier {};
class RefTypeModifier : public AccessTypeModifier {
public:
  RefTypeModifier(TypeModifierScope scope) : AccessTypeModifier(scope) {}
};
// class ArrayTypeModifier : public AccessTypeModifier {};

/// Attributes that may be applied to declarations.
class TypeModifierList {
  /// Linked list of declaration attributes.
  TypeModifier *modifiers;
};

} // namespace stone

#endif