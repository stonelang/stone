#ifndef STONE_AST_DECLSTATE_H
#define STONE_AST_DECLSTATE_H

#include "stone/AST/ASTAllocation.h"
#include "stone/AST/DeclName.h"

#include "stone/AST/Property.h"
#include "stone/AST/TypeAlignment.h"

#include "llvm/ADT/BitVector.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/Support/Allocator.h"

#include <cassert>

namespace stone {

class TypeState;

class alignas(1 << DeclAlignInBits) DeclState final
    : public ASTAllocation<DeclState>,
      public PropertyContext<DeclProperty> {

  // The ASTContext associated with this DeclSate
  ASTContext &astContext;

  // The TypeState assoicated with the DeclState
  TypeState *typeState = nullptr;

  // The declaration name
  DeclName declName;

public:
  // Every DeclState must have a context

  DeclState(ASTContext &AC) : astContext(astContext) {}

public:
  ASTContext &GetASTContext() { return astContext; }

  void SetTypeState(TypeState *TS) { typeState = TS; }
  TypeState *GetTypeState() { return typeState; }

  void SetDeclName(DeclName name) { declName = name; }
  DeclName GetDeclName() { return declName; }

private:
  // Direct comparison is disabled for states
  void operator==(DeclState D) const = delete;
  void operator!=(DeclState D) const = delete;
};

} // namespace stone
#endif
