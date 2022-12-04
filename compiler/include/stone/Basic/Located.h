#ifndef STONE_BASIC_LOCATED_H
#define STONE_BASIC_LOCATED_H

#include "stone/Basic/LLVM.h"
#include "stone/Basic/SrcLoc.h"

namespace stone {

/// A currency type for keeping track of Items which were found in the source
/// code. Several parts of the compiler need to keep track of a `SrcLoc`
/// corresponding to an Item, in case they need to report some diagnostics
/// later. For example, the ClangImporter needs to keep track of where imports
/// were originally written. Located makes it easy to do so while making the
/// code more readable, compared to using `std::pair`.
template <typename T> struct Located {
  /// The main Item whose source location is being tracked.
  T Item;

  /// The original source location from which the Item was parsed.
  SrcLoc Loc;

  Located() : Item(), Loc() {}

  Located(T item, SrcLoc loc) : Item(item), Loc(loc) {}

  // STONE_DEBUG_DUMP;
  // void dump(raw_ostream &os) const;
};

template <typename T>
bool operator==(const Located<T> &lhs, const Located<T> &rhs) {
  return lhs.Item == rhs.Item && lhs.Loc == rhs.Loc;
}

} // end namespace stone

namespace llvm {

template <typename T> struct DenseMapInfo;

template <typename T> struct DenseMapInfo<stone::Located<T>> {

  static inline stone::Located<T> getEmptyKey() {
    return stone::Located<T>(DenseMapInfo<T>::getEmptyKey(),
                             DenseMapInfo<stone::SrcLoc>::getEmptyKey());
  }

  static inline stone::Located<T> getTombstoneKey() {
    return stone::Located<T>(DenseMapInfo<T>::getTombstoneKey(),
                             DenseMapInfo<stone::SrcLoc>::getTombstoneKey());
  }

  static unsigned getHashValue(const stone::Located<T> &LocatedVal) {
    return combineHashValue(
        DenseMapInfo<T>::getHashValue(LocatedVal.Item),
        DenseMapInfo<stone::SrcLoc>::getHashValue(LocatedVal.Loc));
  }

  static bool isEqual(const stone::Located<T> &LHS,
                      const stone::Located<T> &RHS) {
    return DenseMapInfo<T>::isEqual(LHS.Item, RHS.Item) &&
           DenseMapInfo<T>::isEqual(LHS.Loc, RHS.Loc);
  }
};
} // namespace llvm

#endif // STONE_BASIC_LOCATED_H
