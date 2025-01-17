#ifndef STONE_AST_PROPERTY_H
#define STONE_AST_PROPERTY_H

#include "stone/AST/ASTAllocation.h"
#include "stone/AST/AttributeKind.h"
#include "stone/AST/Identifier.h"
#include "stone/AST/TypeAlignment.h"
#include "stone/Basic/SrcLoc.h"

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

public:
  Property(PropertyKind kind, SrcLoc loc) : kind(kind), loc(loc) {}
};

class ModifierProperty : public Property {
public:
  ModifierProperty(PropertyKind kind, SrcLoc loc) : Property(kind, loc) {}
};

class DeclModifier : public ModifierProperty {
public:
  DeclModifier(PropertyKind kind, SrcLoc loc) : ModifierProperty(kind, loc) {}
};

class AttributeProperty : public Property {
public:
  AttributeProperty(PropertyKind kind, SrcLoc loc) : Property(kind, loc) {}
};

class DeclAttribute : public AttributeProperty {
public:
  DeclAttribute(PropertyKind kind, SrcLoc loc) : AttributeProperty(kind, loc) {}
};

} // namespace stone

#endif