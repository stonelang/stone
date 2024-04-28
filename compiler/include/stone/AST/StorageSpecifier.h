#ifndef STONE_SYNTAX_STORAGE_SPECIFIER_H
#define STONE_SYNTAX_STORAGE_SPECIFIER_H

#include "stone/AST/Attribute.h"
#include "stone/Basic/OptionSet.h"

#include "llvm/ADT/ArrayRef.h"

namespace stone {

/// The storage duration for an object (per C++ [ctx.stc]).
enum class StorageDurationKind : UInt8 {
  None = 0,
  FullExpression, ///< Full-expression storage duration (for temporaries).
  Automatic,      ///< Automatic storage duration (most local variables).
  Thread,         ///< Thread storage duration.
  Static,         ///< Static storage duration.
  Dynamic         ///< Dynamic storage duration.
};

/// Storage classes.
/// These are legal on both functions and variables
enum class StorageSpecifierKind : UInt8 {
  None = 0, ///< TODO: You may not need extern
  Extern,   ///< functions
  Static,   ///< Variables, and Types.
  Register  ///< Legal only on variables.
};

class ScopeSpecifier {
  SrcRange range;
  // NestedNameSpecifierLocBuilder builder;

public:
  SrcRange GetRange() const { return range; }
  void SetRange(SrcRange R) { range = R; }
  void SetBeginLoc(SrcLoc loc) { range.Start = loc; }
  void SetEndLoc(SrcLoc loc) { range.End = loc; }
  SrcLoc GetBeginLoc() const { return range.Start; }
  SrcLoc GetEndLoc() const { return range.End; }

  // /// Retrieve the representation of the nested-name-specifier.
  // NestedNameSpecifier *GetScopeRepresentation() const {
  //   return builder.GetRepresentation();
  // }
};

class StorageSpecifierCollector final {

  SrcLoc loc;
  StorageSpecifierKind storageKind;
  StorageDurationKind durationKind;

private:
  void AddStorageSpecifier(StorageSpecifierKind inputKind, SrcLoc inputLoc) {
    storageKind = inputKind;
    loc = inputLoc;
  }
  void AddStorageDurationKind(StorageDurationKind inputKind) {
    durationKind = inputKind;
  }

public:
  StorageSpecifierCollector()
      : storageKind(StorageSpecifierKind::None),
        durationKind(StorageDurationKind::None) {}

public:
  StorageSpecifierKind GetStorageKind() { return storageKind; }
  StorageDurationKind GetDurationKind() { return durationKind; }

  void AddExtern(SrcLoc loc) {
    AddStorageSpecifier(StorageSpecifierKind::Extern, loc);
  }
  bool HasExtern() {
    return (storageKind == StorageSpecifierKind::Extern && loc.isValid());
  }

  void AddStatic(SrcLoc loc) {
    AddStorageSpecifier(StorageSpecifierKind::Static, loc);
  }
  bool HasStatic() {
    return (storageKind == StorageSpecifierKind::Static && loc.isValid());
  }

  void AddRegister(SrcLoc loc) {
    AddStorageSpecifier(StorageSpecifierKind::Register, loc);
  }
  bool HasRegister() {
    return (storageKind == StorageSpecifierKind::Register && loc.isValid());
  }
  bool HasAny() {
    /// TODO: Consider auto
    return (HasExtern() || HasStatic() || HasRegister());
  }

  SrcLoc GetLoc() { return loc; }

  void AddFullExpressionStorageDuration() {
    AddStorageDurationKind(StorageDurationKind::FullExpression);
  }
  void AddAutomaticStorageDuration() {
    AddStorageDurationKind(StorageDurationKind::Automatic);
  }
  void AddThreadStorageDuration() {
    AddStorageDurationKind(StorageDurationKind::Thread);
  }
  void AddStaticStorageDuration() {
    AddStorageDurationKind(StorageDurationKind::Static);
  }
  void AddDynamicStorageDuration() {
    AddStorageDurationKind(StorageDurationKind::Dynamic);
  }
  void Apply();
};

} // namespace stone
#endif