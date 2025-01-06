#ifndef STONE_AST_PROLOGUE_H
#define STONE_AST_PROLOGUE_H

#include "stone/AST/Identifier.h"
#include "stone/AST/PrologueKind.h"
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

class alignas(1 << ModifiersAlignInBits) Modifier
    : public ASTAllocation<Modifier> {};

};
class DeclModifier : public Modifier {
  DeclModifier *Next = nullptr;
};

class StorageDeclModifier : public DeclModifier {};
class StaticDeclModifier : public StorageDeclModifier {};
class ExternDeclModifier : public StorageDeclModifier {};

class VisibilityDeclModifier : public DeclModifier {};
class PublicDeclModifier : public VisibilityModifier {};
class ProtectedDeclModifier : public VisibilityModifier {};
class PrivateModifier : public VisibilityModifier {};

/// Attributes that may be applied to declarations.
class DeclModifierList {
  /// Linked list of declaration attributes.
  DeclModifier *modifiers;
};

class TypeModifier : public Modifier {
  TypeModifier *Next = nullptr;
};

class MutableTypeModifier : public TypeModifier {};
class VolatileTypeModifier : public TypeModifier {};
class ConstTypeModifier : public TypeModifier {};
class PureTypeModifier : public TypeModifier {};
class StoneTypeModifier : public TypeModifier {};

class AccessTypeModifier : public TypeModifier {};
class PtrTypeModifier : public AccessTypeModifier {};
class OwnTypeModifier : public AccessTypeModifier {};
class MoveTypeModifier : public AccessTypeModifier {};
class RefTypeModifier : public AccessTypeModifier {};
class ArrayTypeModifier : public AccessTypeModifier {};

/// Attributes that may be applied to declarations.
class TypeModifierList {
  /// Linked list of declaration attributes.
  TypeModifier *modifiers;
};

} // namespace stone

#endif