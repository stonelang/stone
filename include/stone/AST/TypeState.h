#ifndef STONE_AST_TYPESTATE_H
#define STONE_AST_TYPESTATE_H

#include "stone/AST/DeclState.h"
#include "stone/AST/Property.h"
#include "stone/AST/TypeMetadata.h"
#include "stone/AST/TypeModifier.h"
#include "stone/AST/TypeWalker.h"
#include "stone/Basic/LLVM.h"
#include "stone/Basic/SrcLoc.h"

#include <stdint.h>

namespace stone {

class Type;
class DeclState;

enum class TypeStateKind : uint8_t {
  None = 0,
#define TYPESTATE(ID, PARENT) ID,
#define LAST_TYPESTATE(ID) Last_TypeState = ID,
#include "TypeStateNode.def"
};

class TypeStateFlags {
  friend class ASTContext;
  friend class TypeState;

public:
  enum ID : uint8_t {
    None = 1 << 0,        // No state
    Parsed = 1 << 1,      // Type has been parsed
    Resolved = 1 << 2,    // Type has been resolved
    Unresolved = 1 << 3,  // Type requires resolution
    TypeChecked = 1 << 4, // Type is canonical
    Invalid = 1 << 5      // Type is invalid
  };

  ID flags;

public:
  explicit TypeStateFlags() { AddFlag(TypeStateFlags::None); }
  explicit operator bool() const { !HasFlag(TypeStateFlags::None); }

public:
  bool HasFlag(TypeStateFlags::ID f) const { return (flags & f) == f; }
  bool HasParsed() { return HasFlag(TypeStateFlags::Parsed); }
  bool HasResolved() { return HasFlag(TypeStateFlags::Resolved); }
  bool HasUnresolved() { return HasFlag(TypeStateFlags::Unresolved); }
  bool HasTypeChecked() { return HasFlag(TypeStateFlags::TypeChecked); }
  bool HasInvalid() { return HasFlag(TypeStateFlags::Invalid); }

public:
  void AddFlag(TypeStateFlags::ID f) {
    flags = static_cast<TypeStateFlags::ID>(flags | f);
  }
  void AddParsed() { AddFlag(TypeStateFlags::Parsed); }
  void AddResolved() { AddFlag(TypeStateFlags::Resolved); }
  void AddUnresolved() { AddFlag(TypeStateFlags::Unresolved); }
  void AddTypeChecked() { AddFlag(TypeStateFlags::TypeChecked); }
  void AddInvalid() { AddFlag(TypeStateFlags::Invalid); }

public:
  void SetFlag(TypeStateFlags::ID f) { flags = f; }
  void SetParsed() { SetFlag(TypeStateFlags::Parsed); }
  void SetResolved() { SetFlag(TypeStateFlags::Resolved); }
  void SetUnresolved() { SetFlag(TypeStateFlags::Unresolved); }
  void SetTypeChecked() { SetFlag(TypeStateFlags::TypeChecked); }
  void SetInvalid() { SetFlag(TypeStateFlags::Invalid); }

public:
  void RemoveFlag(TypeStateFlags::ID f) {
    flags = static_cast<TypeStateFlags::ID>(flags & ~f);
  }
  void RemoveParsed() { RemoveFlag(TypeStateFlags::Parsed); }
  void RemoveResolved() { RemoveFlag(TypeStateFlags::Resolved); }
  void RemoveUnresolved() { RemoveFlag(TypeStateFlags::Unresolved); }
  void RemoveTypeChecked() { RemoveFlag(TypeStateFlags::TypeChecked); }
  void RemoveInvalid() { RemoveFlag(TypeStateFlags::Invalid); }
};

class alignas(1 << TypeAlignInBits) TypeState
    : public ASTAllocation<TypeState> {

  friend class ASTContext;

  TypeStateKind kind; // The kind of TypeState (Builtin, Function, etc.)
  SrcLoc typeLoc;     // Source location of the type

  Type *owningType = nullptr; // The Type associated with this TypeState

  TypeMetadata *typeMetadata = nullptr; // Metadata related to the TypeState

  TypeStateFlags stateFlags;       // Flags for type properties

  TypeModifierFlags modifierFlags; // Type modifiers (const, pure, etc.)

  PropertyList<TypeProperty> typeProperties; // Properties for this TypeState
  
  DeclState *declState = nullptr; // The DeclState that owns this TypeState

public:
  explicit TypeState(TypeStateKind kind, SrcLoc loc)
      : kind(kind), typeLoc(loc) {}

public:
  void AddTypeProperty(TypeProperty *property) {
    // switch (mod->GetKind()) {
    // case PropertyKind::Const:
    //   if (modifierFlags.HasConst()) {
    //     return; // Avoid adding duplicate
    //   }
    //   modifierFlags.AddModifier(TypeModifierFlags::Const);
    //   break;

    // case PropertyKind::Pure:
    //   if (modifierFlags.HasPure()) {
    //     return; // Avoid adding duplicate
    //   }
    //   modifierFlags.AddModifier(TypeModifierFlags::Pure);
    //   break;

    // default:
    //   assert(false && "Unknown modifier-property!");
    // }

    // AddProperty(mod); // Add to the property list for metadata and
    // diagnostics
  }

  // void AddTypeAttribute(TypeAttribute *attr) {}

  PropertyList<TypeProperty> &GetTypeProperties() { return typeProperties; }

public:
  Type *GetType() const { return owningType; }
  void SetType(Type *ty) {
    assert(ty && "TypeState cannot be assigned a null Type!");
    owningType = ty;
  }

  void SetLoc(SrcLoc loc) { typeLoc = loc; }
  SrcLoc GetLoc() { return typeLoc; }

  void SetDeclState(DeclState *D) { declState = D; }
  DeclState *GetDeclState() const { return declState; }

public:
  bool HasConst() const { return modifierFlags.HasConst(); }
  bool HasPure() const { return modifierFlags.HasPure(); }
};

class BuiltinTypeState final : public TypeState {

public:
  BuiltinTypeState(SrcLoc loc) : TypeState(TypeStateKind::Builtin, loc) {}

public:
  // Directly set the Type object instead of managing TypeKind
  void SetBuiltinType(Type *type) { SetType(type); }

  // Retrieve the type directly from the base TypeState
  Type *GetBuiltinType() const { return GetType(); }
};

class IdentifierTypeState : public TypeState {
public:
  IdentifierTypeState(TypeStateKind kind, SrcLoc loc) : TypeState(kind, loc) {}
};

class SimpleIdentifierTypeState : public IdentifierTypeState {
public:
  SimpleIdentifierTypeState(SrcLoc loc)
      : IdentifierTypeState(TypeStateKind::SimpleIdentifier, loc) {}
};

class FunctionTypeState : public TypeState {

  SrcLoc arrowLoc;
  SrcLoc lParenLoc;
  SrcLoc rParenLoc;

  TypeState *resultTypeState = nullptr;

public:
  FunctionTypeState(SrcLoc loc) : TypeState(TypeStateKind::Function, loc) {}

public:
  void SetResultTypeState(TypeState *typeState) { resultTypeState = typeState; }
  TypeState *GetResultTypeState() { return resultTypeState; }

  // void SetBody(BraceStmt *BS) { bodyStmt = BS; }
  // BraceStmt *GetBody() { return bodyStmt; }

  void SetArrow(SrcLoc loc) { arrowLoc = loc; }
  SrcLoc GetArrow() { return arrowLoc; }
  bool HasArrow() { return GetArrow().isValid(); }

  void SetLParen(SrcLoc loc) { lParenLoc = loc; }
  SrcLoc GetLParen() { return lParenLoc; }
  bool HasLParen() { return GetLParen().isValid(); }

  void SetRParen(SrcLoc loc) { rParenLoc = loc; }
  SrcLoc GetRParen() { return rParenLoc; }
  bool HasRParen() { return GetRParen().isValid(); }
};

} // namespace stone
#endif
