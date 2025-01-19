#ifndef STONE_AST_TYPESTATE_H
#define STONE_AST_TYPESTATE_H

#include "stone/AST/DeclState.h"
#include "stone/AST/Property.h"
#include "stone/AST/TypeMetadata.h"
#include "stone/AST/TypeWalker.h"
#include "stone/Basic/LLVM.h"
#include "stone/Basic/SrcLoc.h"

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/APInt.h"
#include "llvm/ADT/APSInt.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/DenseMapInfo.h"
#include "llvm/ADT/FoldingSet.h"
#include "llvm/ADT/Hashing.h"
#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/PointerUnion.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/Twine.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/PointerLikeTypeTraits.h"
#include "llvm/Support/TrailingObjects.h"
#include "llvm/Support/type_traits.h"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <optional>
#include <stdint.h>
#include <string>
#include <type_traits>
#include <utility>

namespace stone {
class Type;

enum class TypeStateKind : uint8_t {
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
    : public ASTAllocation<std::aligned_storage<8, 8>::type> {
  friend class ASTContext;
  // friend TrailingObjects;

  /// The specific type-state
  TypeStateKind kind;

  DeclState *owningDeclState;

  PropertyCollector<TypeProperty> typeProperties;

  // /// The location of the type
  SrcLoc loc;

  // The type that we are processing
  Type *typePtr = nullptr;

  // /// The owning DeclStae
  // DeclState *declState = nullptr;

  // // Efficient memory allocation
  // llvm::BumpPtrAllocator allocator;

  // // Stores properties with metadata
  // llvm::DenseMap<PropertyKind, TypeProperty *> properties;

  // // Tracks property presence
  // llvm::BitVector propertyMask;

  // size_t numTrailingDecls = 0;      // Number of trailing Decl* objects
  // size_t numTrailingTypeStates = 0; // Number of trailing TypeState* objects

public:
  TypeStateFlags Status;

public:
  explicit TypeState(TypeStateKind kind) : kind(kind) {}

public:
  Type *GetType() const { return typePtr; }
  void SetType(Type *t) { typePtr = t; }

  void SetLoc(SrcLoc L) { loc = L; }
  SrcLoc GetLoc() const { return loc; }
  // SrcRange GetSrcRange() const;

  // DeclState *GetDeclState() const { return declState; }
  // void SetDeclState(DeclState *DS) { declState = DS; }

public:
  // // Access to trailing Decl* objects
  // ArrayRef<Decl *> GetTrailingDecls() const {
  //   return {getTrailingObjects<Decl *>(), numTrailingDecls};
  // }

  // MutableArrayRef<Decl *> GetTrailingDecls() {
  //   return {getTrailingObjects<Decl *>(), numTrailingDecls};
  // }

  // // Access to trailing TypeState* objects
  // ArrayRef<TypeState *> GetTrailingTypeStates() const {
  //   return {getTrailingObjects<TypeState *>(), numTrailingTypeStates};
  // }

  // MutableArrayRef<TypeState *> GetTrailingTypeStates() {
  //   return {getTrailingObjects<TypeState *>(), numTrailingTypeStates};
  // }

public:
  // void AddTypeProperty(PropertyKind kind, TypeProperty *property) {
  //   typeProperties[kind] = property;
  //   typePropertyMask.set(static_cast<size_t>(kind));
  // }

  // TypeProperty *GetTypeProperty(PropertyKind kind) const {
  //   auto it = typeProperties.find(kind);
  //   return (it != typeProperties.end()) ? it->second : nullptr;
  // }

  // bool HasTypeProperty(PropertyKind kind) const {
  //   return typePropertyMask.test(static_cast<size_t>(kind));
  // }

public:
  /// Walk this Type.
  ///
  /// Returns true if the walk was aborted.
  bool Walk(TypeWalker &walker) const;
  bool Walk(TypeWalker &&walker) const { return Walk(walker); }

private:
  // Direct comparison is disabled for types, because they may not be canonical.
  void operator==(TypeState T) const = delete;
  void operator!=(TypeState T) const = delete;
};

class ModuleTypeState : public TypeState {
public:
  ModuleTypeState() : TypeState(TypeStateKind::Module) {}
};

// class FunctionTypeState : public TypeState {
// public:
//   FunctionTypeState(SrcLoc loc) : TypeState(TypeStateKind::Function) {}

// public:
// void AddParamDecls(ArrayRef<Decl *> paramDecls) {
//   SetTrailingDecls(paramDecls);
// }

// void AddChildTypeStates(ArrayRef<TypeState *> childTypeStates) {
//   SetTrailingTypeStates(childTypeStates);
// }

// // Accessors for parameters and return type
// ArrayRef<Decl *> GetParamDecls() const {
//   return GetTrailingDecls();
// }

// ArrayRef<TypeState *> GetParamTypeStates() const {
//   auto childTypeStates = GetTrailingTypeStates();
//   return childTypeStates.drop_back(1); // Exclude the return type
// }
// };

} // namespace stone
#endif
