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

public:
  bool IsModifier() const;
  bool IsAttribute() const;

public:
  bool IsStatic() const { return kind == PropertyKind::Static; }
  bool IsExern() const { return kind == PropertyKind::Extern; }
  bool IsPublic() const { return kind == PropertyKind::Public; }
  bool IsProtected() const { return kind == PropertyKind::Protected; }
  bool IsPrivate() const { return kind == PropertyKind::Private; }
  bool IsConst() const { return kind == PropertyKind::Const; }
  bool IsStone() const { return kind == PropertyKind::Const; }
  bool IsPure() const { return kind == PropertyKind::Pure; }
  bool IsVolaltile() const { return kind == PropertyKind::Volatile; }

  bool IsDeprecated() const { return kind == PropertyKind::Deprecated; }
  bool IsAligned() const { return kind == PropertyKind::Aligned; }
};

class Modifier : public Property {
public:
  Modifier(PropertyKind kind, SrcLoc loc) : Property(kind, loc) {}
};

class DeclModifier : public Modifier {
public:
  DeclModifier(PropertyKind kind, SrcLoc loc) : Modifier(kind, loc) {}

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

class TypeModifier : public Modifier {
public:
  TypeModifier(PropertyKind kind, SrcLoc loc) : Modifier(kind, loc) {}
};

class ConstModifier : public TypeModifier {
public:
  ConstModifier(SrcLoc loc) : TypeModifier(PropertyKind::Const, loc) {}
};

class Attribute : public Property {
public:
  Attribute(PropertyKind kind, SrcLoc loc) : Property(kind, loc) {}
};

class DeclAttribute : public Attribute {
public:
  DeclAttribute(PropertyKind kind, SrcLoc loc) : Attribute(kind, loc) {}
};

class DeprecatedAttribute : public DeclAttribute {
public:
  DeprecatedAttribute(SrcLoc loc) : DeclAttribute(PropertyKind::Deprecated, loc) {}
};

class TypeAttribute : public Attribute {
public:
  TypeAttribute(PropertyKind kind, SrcLoc loc) : Attribute(kind, loc) {}
};
class AlignAttribute : public TypeAttribute {
public:
  AlignAttribute(SrcLoc loc) : TypeAttribute(PropertyKind::Aligned, loc) {}
};

} // namespace stone

#endif