#ifndef STONE_AST_TYPESTATE_H
#define STONE_AST_TYPESTATE_H

#include "stone/AST/DeclState.h"
#include "stone/AST/TypeInfluencer.h"
#include "stone/AST/TypeMetadata.h"
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

  const ASTContext &astContext;

  // The kind of TypeState (Builtin, Function, etc.)
  TypeStateKind kind;

  // Source location of the type
  SrcLoc typeLoc;

  // The canonical type associated with this TypeSate
  Type *canType = nullptr;

  // Metadata related to the TypeState
  TypeMetadata *typeMetadata = nullptr;

  // Flags for type properties
  TypeStateFlags stateFlags;

  // Properties for this TypeState
  TypeInfluencerList influencers;

  // The DeclState that owns this TypeState
  DeclState *declState = nullptr;

public:
  explicit TypeState(TypeStateKind kind, SrcLoc loc,
                     const ASTContext &astContext)
      : kind(kind), typeLoc(loc), astContext(astContext),
        influencers(astContext) {}

public:
  TypeStateKind GetKind() { return kind; }

  //\return TypeInfluencerList
  TypeInfluencerList &GetTypeInfluencerList() { return influencers; }

  //\return true if the type is FunType
  bool IsBuiltin() { return GetKind() == TypeStateKind::Builtin; }

  //\return true if the type is FunType
  bool IsFunction() { return GetKind() == TypeStateKind::Function; }

  // //\return true if the type is enum type
  // bool IsEnum() const { return GetKind() == TypeStateKind::Enum; }

  // //\return true if the type is struct type
  // bool IsStruct() const { return GetKind() == TypeStateKind::Struct; }

  // //\return true if the type is class type
  // bool IsClass() const { return GetKind() == TypeStateKind::Class; }

public:
  Type *GetCanType() const { return canType; }

  void SetCanType(Type *ty) {
    assert(ty && "TypeState cannot be assigned a null Type!");
    canType = ty;
  }

  void SetLoc(SrcLoc loc) { typeLoc = loc; }
  SrcLoc GetLoc() { return typeLoc; }

  void SetDeclState(DeclState *D) { declState = D; }
  DeclState *GetDeclState() const { return declState; }
};

class BuiltinTypeState final : public TypeState {

public:
  BuiltinTypeState(const ASTContext &astContext)
      : TypeState(TypeStateKind::Builtin, SrcLoc(), astContext) {}
};

class IdentifierTypeState : public TypeState {
public:
  IdentifierTypeState(TypeStateKind kind, SrcLoc loc,
                      const ASTContext &astContext)
      : TypeState(kind, loc, astContext) {}
};

class SimpleIdentifierTypeState : public IdentifierTypeState {
public:
  SimpleIdentifierTypeState(SrcLoc loc, const ASTContext &astContext)
      : IdentifierTypeState(TypeStateKind::SimpleIdentifier, loc, astContext) {}
};

class FunctionTypeStateOptions final {
public:
  enum Kind : unsigned {
    None = 0,
    ForcedInline = 1 << 1,
  };

private:
  unsigned options = Kind::None;

public:
  bool HasOption(FunctionTypeStateOptions::Kind option) const {
    return (options & option) == option;
  }
  void AddOption(FunctionTypeStateOptions::Kind option) {
    options = static_cast<FunctionTypeStateOptions::Kind>(options | option);
  }
  void RemoveOption(FunctionTypeStateOptions::Kind option) {
    options = static_cast<FunctionTypeStateOptions::Kind>(options & ~option);
  }
};

class FunctionTypeState : public TypeState {

  SrcLoc arrowLoc;
  SrcLoc lParenLoc;
  SrcLoc rParenLoc;

  TypeState *resultTypeState = nullptr;

  FunctionTypeStateOptions options;

public:
  enum class CallingConvention : uint8 { Stone, C };

private:
  CallingConvention callingConvention;

public:
  FunctionTypeState(SrcLoc loc, const ASTContext &astContext)
      : TypeState(TypeStateKind::Function, loc, astContext) {}

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

  FunctionTypeStateOptions &GetOptions() { return options; }

  void SetCallingConvention(FunctionTypeState::CallingConvention convention) {
    callingConvention = convention;
  }
  FunctionTypeState::CallingConvention GetCallingConvention() {
    return callingConvention;
  }
};

} // namespace stone
#endif
