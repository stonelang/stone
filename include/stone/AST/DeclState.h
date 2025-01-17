#ifndef STONE_AST_DECLSTATE_H
#define STONE_AST_DECLSTATE_H

#include "stone/AST/ASTAllocation.h"
#include "stone/AST/Decl.h"
#include "stone/AST/DeclState.h"
#include "stone/AST/Property.h"
#include "stone/AST/TypeAlignment.h"

#include "llvm/ADT/BitVector.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/Support/Allocator.h"
#include <cassert>

namespace stone {

class alignas(1 << DeclAlignInBits) DeclState final : ASTAllocation<DeclState> {
  friend class ASTContext;

public:
  Decl *D = nullptr;
  SrcLoc kindLoc;

  // Efficient memory allocation
  llvm::BumpPtrAllocator allocator;
  // Stores properties with metadata
  llvm::DenseMap<PropertyKind, Property *> properties;
  // Tracks property presence
  llvm::BitVector propertyMask;

public:
  DeclState() : DeclState(nullptr) {}

  explicit DeclState(Decl *decl)
      : D(decl),
        propertyMask(static_cast<size_t>(PropertyKind::Last_Type) + 1) {}

  // Overload the bool operator
  explicit operator bool() const { return D != nullptr; }

public:
  void SetDecl(Decl *decl) { D = decl; }
  Decl *GetDecl() { return D; }

private:
  // Direct comparison is disabled for states
  void operator==(DeclState D) const = delete;
  void operator!=(DeclState D) const = delete;
};

} // namespace stone
#endif
