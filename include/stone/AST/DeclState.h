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
class ASTContext;

class alignas(1 << DeclAlignInBits) DeclState final : ASTAllocation<DeclState> {
  friend ASTContext;

  // The ASTContext of the Decl
  ASTContext *AC = nullptr;

  // Efficient memory allocation
  llvm::BumpPtrAllocator allocator;
  
  // Stores properties with metadata
  llvm::DenseMap<PropertyKind, DeclProperty *> properties;

  // Tracks property presence
  llvm::BitVector propertyMask;

  // Direct comparison is disabled for states
  void operator==(DeclState D) const = delete;
  void operator!=(DeclState D) const = delete;

public:
  DeclState() : DeclState(nullptr) {}
  explicit DeclState(ASTContext *AC)
      : AC(AC), propertyMask(static_cast<size_t>(PropertyKind::Last_Type) + 1) {
  }

public:
  // Overload the bool operator
  explicit operator bool() const { return !properties.empty(); }
  ASTContext *GetASTContext() { return AC; }
  bool HasASTContext() { return AC != nullptr; }

public:
  // Add a property to the state if it doesn't already exist
  template <typename T, typename... Args>
  T *AddProperty(PropertyKind kind, Args &&...args) {
    // Check if the property already exists
    if (HasProperty(kind)) {
      return static_cast<T *>(GetProperty(kind));
    }

    // Allocate memory for the property using the allocator
    T *property =
        new (allocator.Allocate<T>()) T(kind, std::forward<Args>(args)...);
    properties[kind] = property;

    // Mark the property as present in the BitVector
    propertyMask.set(static_cast<size_t>(kind));
    return property;
  }

  // Check if a property is present (using BitVector)
  bool HasProperty(PropertyKind kind) const {
    return propertyMask.test(static_cast<size_t>(kind));
  }

  // Retrieve a property by kind (from DenseMap)
  DeclProperty *GetProperty(PropertyKind kind) const {
    auto it = properties.find(kind);
    return (it != properties.end()) ? it->second : nullptr;
  }

  template <typename Callback>
  void ForEachProperty(
      Callback &&callback,
      PropertyAvailability filter = PropertyAvailability::None) const {
    for (const auto &[kind, property] : properties) {
      if (filter == PropertyAvailability::None ||
          property->GetAvailability() == filter) {
        callback(kind, property);
      }
    }
  }
};

} // namespace stone
#endif
