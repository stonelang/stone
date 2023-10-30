#ifndef STONE_BASIC_DIRECTORYENTRY_H
#define STONE_BASIC_DIRECTORYENTRY_H

#include "stone/Basic/LLVM.h"
#include "llvm/ADT/DenseMapInfo.h"
#include "llvm/ADT/Hashing.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/ErrorOr.h"

namespace stone {
namespace fm {

template <class RefTy> class MapEntryOptionalStorage;

} // end namespace fm

/// Cached information about one directory (either on disk or in
/// the virtual file system).
class DirEntry {
  friend class FileMgr;

  // FIXME: We should not be storing a directory entry name here.
  StringRef Name; // Name of the directory.

public:
  StringRef getName() const { return Name; }
};

/// A reference to a \c DirEntry  that includes the name of the directory
/// as it was accessed by the FileMgr's client.
class DirEntryRef {
public:
  const DirEntry &getDirEntry() const { return *ME->getValue(); }

  StringRef getName() const { return ME->getKey(); }

  /// Hash code is based on the DirEntry, not the specific named
  /// reference.
  friend llvm::hash_code hash_value(DirEntryRef Ref) {
    return llvm::hash_value(&Ref.getDirEntry());
  }

  using MapEntry = llvm::StringMapEntry<llvm::ErrorOr<DirEntry &>>;

  const MapEntry &getMapEntry() const { return *ME; }

  /// Check if RHS referenced the file in exactly the same way.
  bool isSameRef(DirEntryRef RHS) const { return ME == RHS.ME; }

  DirEntryRef() = delete;
  DirEntryRef(const MapEntry &ME) : ME(&ME) {}

  /// Allow DirEntryRef to degrade into 'const DirEntry*' to
  /// facilitate incremental adoption.
  ///
  /// The goal is to avoid code churn due to dances like the following:
  /// \code
  /// // Old code.
  /// lvalue = rvalue;
  ///
  /// // Temporary code from an incremental patch.
  /// lvalue = &rvalue.getDirEntry();
  ///
  /// // Final code.
  /// lvalue = rvalue;
  /// \endcode
  ///
  /// FIXME: Once DirEntryRef is "everywhere" and DirEntry::getName
  /// has been deleted, delete this implicit conversion.
  operator const DirEntry *() const { return &getDirEntry(); }

private:
  friend class stone::fm::MapEntryOptionalStorage<DirEntryRef>;
  struct optional_none_tag {};

  // Private constructor for use by OptionalStorage.
  DirEntryRef(optional_none_tag) : ME(nullptr) {}
  bool hasOptionalValue() const { return ME; }

  friend struct llvm::DenseMapInfo<DirEntryRef>;
  struct dense_map_empty_tag {};
  struct dense_map_tombstone_tag {};

  // Private constructors for use by DenseMapInfo.
  DirEntryRef(dense_map_empty_tag)
      : ME(llvm::DenseMapInfo<const MapEntry *>::getEmptyKey()) {}
  DirEntryRef(dense_map_tombstone_tag)
      : ME(llvm::DenseMapInfo<const MapEntry *>::getTombstoneKey()) {}
  bool isSpecialDenseMapKey() const {
    return isSameRef(DirEntryRef(dense_map_empty_tag())) ||
           isSameRef(DirEntryRef(dense_map_tombstone_tag()));
  }

  const MapEntry *ME;
};

namespace fm {

/// Customized storage for refs derived from map entires in FileMgr, using
/// the private optional_none_tag to keep it to the size of a single pointer.
template <class RefTy> class MapEntryOptionalStorage {
  using optional_none_tag = typename RefTy::optional_none_tag;
  RefTy MaybeRef;

public:
  MapEntryOptionalStorage() : MaybeRef(optional_none_tag()) {}

  template <class... ArgTypes>
  explicit MapEntryOptionalStorage(llvm::in_place_t, ArgTypes &&...Args)
      : MaybeRef(std::forward<ArgTypes>(Args)...) {}

  void reset() { MaybeRef = optional_none_tag(); }

  bool has_value() const { return MaybeRef.hasOptionalValue(); }
  bool hasValue() const { return MaybeRef.hasOptionalValue(); }

  RefTy &value() & {
    assert(has_value());
    return MaybeRef;
  }
  RefTy &getValue() & {
    assert(has_value());
    return MaybeRef;
  }
  RefTy const &value() const & {
    assert(has_value());
    return MaybeRef;
  }
  RefTy const &getValue() const & {
    assert(has_value());
    return MaybeRef;
  }
  RefTy &&value() && {
    assert(has_value());
    return std::move(MaybeRef);
  }
  RefTy &&getValue() && {
    assert(has_value());
    return std::move(MaybeRef);
  }

  template <class... Args> void emplace(Args &&...args) {
    MaybeRef = RefTy(std::forward<Args>(args)...);
  }

  MapEntryOptionalStorage &operator=(RefTy Ref) {
    MaybeRef = Ref;
    return *this;
  }
};

} // end namespace fm
} // end namespace stone

namespace llvm {
namespace optional_detail {

/// Customize OptionalStorage<DirEntryRef> to use DirEntryRef and
/// its optional_none_tag to keep it the size of a single pointer.
template <>
class OptionalStorage<stone::DirEntryRef>
    : public stone::fm::MapEntryOptionalStorage<stone::DirEntryRef> {
  using StorageImpl = stone::fm::MapEntryOptionalStorage<stone::DirEntryRef>;

public:
  OptionalStorage() = default;

  template <class... ArgTypes>
  explicit OptionalStorage(in_place_t, ArgTypes &&...Args)
      : StorageImpl(in_place_t{}, std::forward<ArgTypes>(Args)...) {}

  OptionalStorage &operator=(stone::DirEntryRef Ref) {
    StorageImpl::operator=(Ref);
    return *this;
  }
};

static_assert(sizeof(llvm::Optional<stone::DirEntryRef>) ==
                  sizeof(stone::DirEntryRef),
              "llvm::Optional<DirEntryRef> must avoid size overhead");

static_assert(
    std::is_trivially_copyable<llvm::Optional<stone::DirEntryRef>>::value,
    "llvm::Optional<DirEntryRef> should be trivially copyable");

} // end namespace optional_detail

/// Specialisation of DenseMapInfo for DirEntryRef.
template <> struct DenseMapInfo<stone::DirEntryRef> {
  static inline stone::DirEntryRef getEmptyKey() {
    return stone::DirEntryRef(stone::DirEntryRef::dense_map_empty_tag());
  }

  static inline stone::DirEntryRef getTombstoneKey() {
    return stone::DirEntryRef(stone::DirEntryRef::dense_map_tombstone_tag());
  }

  static unsigned getHashValue(stone::DirEntryRef Val) {
    return hash_value(Val);
  }

  static bool isEqual(stone::DirEntryRef LHS, stone::DirEntryRef RHS) {
    // Catch the easy cases: both empty, both tombstone, or the same ref.
    if (LHS.isSameRef(RHS))
      return true;

    // Confirm LHS and RHS are valid.
    if (LHS.isSpecialDenseMapKey() || RHS.isSpecialDenseMapKey())
      return false;

    // It's safe to use operator==.
    return LHS == RHS;
  }
};

} // end namespace llvm

namespace stone {

/// Wrapper around llvm::Optional<DirEntryRef> that degrades to 'const
/// DirEntry*', facilitating incremental patches to propagate
/// DirEntryRef.
///
/// This class can be used as return value or field where it's convenient for
/// an llvm::Optional<DirEntryRef> to degrade to a 'const DirEntry*'. The
/// purpose is to avoid code churn due to dances like the following:
/// \code
/// // Old code.
/// lvalue = rvalue;
///
/// // Temporary code from an incremental patch.
/// llvm::Optional<DirEntryRef> MaybeF = rvalue;
/// lvalue = MaybeF ? &MaybeF.getDirEntry() : nullptr;
///
/// // Final code.
/// lvalue = rvalue;
/// \endcode
///
/// FIXME: Once DirEntryRef is "everywhere" and DirEntry::LastRef
/// and DirEntry::getName have been deleted, delete this class and
/// replace instances with llvm::Optional<DirEntryRef>.
class OptionalDirEntryRefDegradesToDirEntryPtr
    : public llvm::Optional<DirEntryRef> {
public:
  OptionalDirEntryRefDegradesToDirEntryPtr() = default;
  OptionalDirEntryRefDegradesToDirEntryPtr(
      OptionalDirEntryRefDegradesToDirEntryPtr &&) = default;
  OptionalDirEntryRefDegradesToDirEntryPtr(
      const OptionalDirEntryRefDegradesToDirEntryPtr &) = default;
  OptionalDirEntryRefDegradesToDirEntryPtr &
  operator=(OptionalDirEntryRefDegradesToDirEntryPtr &&) = default;
  OptionalDirEntryRefDegradesToDirEntryPtr &
  operator=(const OptionalDirEntryRefDegradesToDirEntryPtr &) = default;

  OptionalDirEntryRefDegradesToDirEntryPtr(llvm::NoneType) {}
  OptionalDirEntryRefDegradesToDirEntryPtr(DirEntryRef Ref)
      : llvm::Optional<DirEntryRef>(Ref) {}
  OptionalDirEntryRefDegradesToDirEntryPtr(llvm::Optional<DirEntryRef> MaybeRef)
      : llvm::Optional<DirEntryRef>(MaybeRef) {}

  OptionalDirEntryRefDegradesToDirEntryPtr &operator=(llvm::NoneType) {
    llvm::Optional<DirEntryRef>::operator=(None);
    return *this;
  }
  OptionalDirEntryRefDegradesToDirEntryPtr &operator=(DirEntryRef Ref) {
    llvm::Optional<DirEntryRef>::operator=(Ref);
    return *this;
  }
  OptionalDirEntryRefDegradesToDirEntryPtr &
  operator=(llvm::Optional<DirEntryRef> MaybeRef) {
    llvm::Optional<DirEntryRef>::operator=(MaybeRef);
    return *this;
  }

  /// Degrade to 'const DirEntry *' to allow  DirEntry::LastRef and
  /// DirEntry::getName have been deleted, delete this class and replace
  /// instances with llvm::Optional<DirEntryRef>
  operator const DirEntry *() const {
    return hasValue() ? &getValue().getDirEntry() : nullptr;
  }
};

static_assert(
    std::is_trivially_copyable<OptionalDirEntryRefDegradesToDirEntryPtr>::value,
    "OptionalDirEntryRefDegradesToDirEntryPtr should be "
    "trivially copyable");

} // end namespace stone

#endif
