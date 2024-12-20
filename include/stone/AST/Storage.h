#ifndef STONE_AST_STORAGE_H
#define STONE_AST_STORAGE_H

#include "stone/AST/Attribute.h"
#include "stone/Basic/OptionSet.h"
#include "llvm/ADT/ArrayRef.h"

namespace stone {

/// The storage duration for an object (per C++ [ctx.stc]).
enum class StorageDurationKind : uint8_t {
  None = 0,
  FullExpression, ///< Full-expression storage duration (for temporaries).
  Automatic,      ///< Automatic storage duration (most local variables).
  Thread,         ///< Thread storage duration.
  Static,         ///< Static storage duration.
  Dynamic         ///< Dynamic storage duration.
};

/// Storage classes.
/// These are legal on both functions and variables
enum class StorageSpecKind : uint8_t {
  None = 0, ///< TODO: You may not need extern
  Auto,
  Extern,  ///< functions
  Static,  ///< Variables, and Types.
  Register ///< Legal only on variables.
};

class StorageSpec final {

  SrcLoc loc;
  StorageSpecKind kind;

  void AddStorageSpec(StorageSpecKind inputKind, SrcLoc specLoc) {
    assert(!HasAny() && "Can only have one storage kind");
    kind = inputKind;
    loc = specLoc;
  }
  bool IsStorageSpec(StorageSpecKind k) { return (kind == k && loc.isValid()); }

  StorageSpec(const StorageSpec &) = delete;
  void operator=(const StorageSpec &) = delete;

public:
  StorageSpec() : kind(StorageSpecKind::None) {}

public:
  StorageSpecKind GetKind() { return kind; }
  SrcLoc GetLoc() { return loc; }

  void AddAuto(SrcLoc loc) { AddStorageSpec(StorageSpecKind::Auto, loc); }
  bool IsAuto() { return IsStorageSpec(StorageSpecKind::Auto); }

  void AddExtern(SrcLoc loc) { AddStorageSpec(StorageSpecKind::Extern, loc); }
  bool IsExtern() { return IsStorageSpec(StorageSpecKind::Extern); }

  void AddStatic(SrcLoc loc) { AddStorageSpec(StorageSpecKind::Static, loc); }
  bool IsStatic() { return IsStorageSpec(StorageSpecKind::Static); }

  void AddRegister(SrcLoc loc) {
    AddStorageSpec(StorageSpecKind::Register, loc);
  }
  bool IsRegister() { return IsStorageSpec(StorageSpecKind::Register); }

  bool HasAny() {
    switch (GetKind()) {
    case StorageSpecKind::Auto:
    case StorageSpecKind::Extern:
    case StorageSpecKind::Register:
    case StorageSpecKind::Static:

      return true;
    }
    return false;
  }
};

class StorageDuration final {

  StorageDurationKind kind;

  void AddKind(StorageDurationKind k) {
    assert(!HasAny() && "Can only have one storage duration kind");
    kind = k;
  }
  bool IsKind(StorageDurationKind k) { return kind == k; }

  StorageDuration(const StorageDuration &) = delete;
  void operator=(const StorageDuration &) = delete;

public:
  StorageDuration() : kind(StorageDurationKind::None) {}

public:
  StorageDurationKind GetKind() { return kind; }

  void AddFullExpression() { AddKind(StorageDurationKind::FullExpression); }
  bool IsFullExpression() {
    return IsKind(StorageDurationKind::FullExpression);
  }

  void AddThread() { AddKind(StorageDurationKind::Thread); }
  bool IsThread() { return IsKind(StorageDurationKind::Thread); }

  void AddStatic() { AddKind(StorageDurationKind::Static); }
  bool IsStatic() { return IsKind(StorageDurationKind::Static); }

  void AddDynamic() { AddKind(StorageDurationKind::Dynamic); }
  bool IsDynamic() { return IsKind(StorageDurationKind::Dynamic); }

  bool HasAny() {
    switch (GetKind()) {
    case StorageDurationKind::FullExpression:
    case StorageDurationKind::Thread:
    case StorageDurationKind::Static:
    case StorageDurationKind::Dynamic:
      return true;
    }
    return false;
  }
};

} // namespace stone
#endif