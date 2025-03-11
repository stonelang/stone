#ifndef STONE_AST_DECLSTATE_H
#define STONE_AST_DECLSTATE_H

#include "stone/AST/ASTAllocation.h"
#include "stone/AST/DeclName.h"

#include "stone/AST/Property.h"
#include "stone/AST/TypeAlignment.h"
// #include "stone/AST/TypeState.h"
#include "llvm/ADT/BitVector.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/Support/Allocator.h"

#include <cassert>

namespace stone {

class Decl;
class TypeState;

class alignas(1 << DeclAlignInBits) DeclState
    : public ASTAllocation<DeclState> {

  // The ASTContext associated with this DeclSate
  ASTContext &astContext;

  // The TypeState assoicated with the DeclState
  TypeState *typeState = nullptr;

  // The properties for the DeclState
  PropertyList<DeclProperty> declProperties;

  // The properties for the TypeState
  PropertyList<TypeProperty> typeProperties;

  // The Decl associated with this DeclState
  Decl *owningDecl = nullptr;

  // The declaration name
  DeclName declName;

public:
  // Every DeclState must have a context
  explicit DeclState(ASTContext &astContext);

public:
  ASTContext &GetASTContext() { return astContext; }

  void SetTypeState(TypeState *TS) { typeState = TS; }
  TypeState *GetTypeState() { return typeState; }

  void SetDecl(Decl *D) { owningDecl = D; }
  Decl *Decl() { return owningDecl; }

  void SetDeclName(DeclName name) { declName = name; }
  DeclName GetDeclName() { return declName; }

  void AddTypeProperty(TypeProperty *property) {}
  PropertyList<TypeProperty> &GetTypeProperties() { return typeProperties; }

  void AddDeclProperty(DeclProperty *property) {}
  PropertyList<DeclProperty> &GetDeclProperties() { return declProperties; }

private:
  // Direct comparison is disabled for states
  void operator==(DeclState D) const = delete;
  void operator!=(DeclState D) const = delete;
};

} // namespace stone
#endif
