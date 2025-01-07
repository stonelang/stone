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

namespace stone {

enum class ModifierKind : uint8_t {
#define MODIFIER(ID, PARENT) ID,
#define LAST_MODIFIER(ID) Last_Modifier = ID,
#include "ModifierKind.def"
};

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

public:
  Modifier(ModifierKind Kind) : Kind(Kind) {}

public:
  ModifierKind GetKind() { return Kind; }
  ModifierScope GetScope() { return scope; }
  void SetScope(ModifierScope S) { scope = S; }
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
// class VolatileTypeModifier : public TypeModifier {};

// class VolatileTypeModifier : public TypeModifier {

// public:
//   VolatileTypeModifier() : TypeModifier(ModifierKind::Volatile) {}
// };

class ConstTypeModifier : public TypeModifier {

public:
  ConstTypeModifier() : TypeModifier(ModifierKind::Const) {}
};
// class PureTypeModifier : public TypeModifier {};
// class StoneTypeModifier : public TypeModifier {};

class AccessTypeModifier : public TypeModifier {
public:
  AccessTypeModifier(ModifierKind kind) : TypeModifier(kind) {}
};
class PointerTypeModifier : public AccessTypeModifier {
public:
  PointerTypeModifier() : AccessTypeModifier(ModifierKind::Pointer) {}
};
// class OwnTypeModifier : public AccessTypeModifier {};
// class MoveTypeModifier : public AccessTypeModifier {};

class ReferenceTypeModifier : public AccessTypeModifier {
public:
  ReferenceTypeModifier() : AccessTypeModifier(ModifierKind::Reference) {}
};

// class ArrayTypeModifier : public AccessTypeModifier {};

/// Attributes that may be applied to declarations.
class TypeModifierList {
  /// Linked list of declaration attributes.
  TypeModifier *modifiers;
};

} // namespace stone

#endif