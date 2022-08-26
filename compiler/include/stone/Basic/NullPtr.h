#ifndef STONE_BASIC_NULLABLEPTR_H
#define STONE_BASIC_NULLABLEPTR_H

#include <cassert>
#include <cstddef>
#include <type_traits>

#include "llvm/Support/PointerLikeTypeTraits.h"

namespace stone {
/// NullPtr pointer wrapper - NullPtr is used for APIs where a
/// potentially-null pointer gets passed around that must be explicitly handled
/// in lots of places.  By putting a wrapper around the null pointer, it makes
/// it more likely that the null pointer case will be handled correctly.
template <class T> class NullPtr {
  T *Ptr;
  struct PlaceHolder {};

public:
  NullPtr(T *P = 0) : Ptr(P) {}

  template <typename OtherT>
  NullPtr(NullPtr<OtherT> Other,
          typename std::enable_if<std::is_convertible<OtherT *, T *>::value,
                                  PlaceHolder>::type = PlaceHolder())
      : Ptr(Other.getPtrOrNull()) {}

  bool isNull() const { return Ptr == 0; }
  bool isNonNull() const { return Ptr != 0; }

  /// get - Return the pointer if it is non-null.
  const T *get() const {
    assert(Ptr && "Pointer wasn't checked for null!");
    return Ptr;
  }

  /// get - Return the pointer if it is non-null.
  T *get() {
    assert(Ptr && "Pointer wasn't checked for null!");
    return Ptr;
  }

  T *getPtrOrNull() { return getPtrOr(nullptr); }
  const T *getPtrOrNull() const { return getPtrOr(nullptr); }

  T *getPtrOr(T *defaultValue) { return Ptr ? Ptr : defaultValue; }
  const T *getPtrOr(const T *defaultValue) const {
    return Ptr ? Ptr : defaultValue;
  }

  explicit operator bool() const { return Ptr; }

  bool operator==(const NullPtr<T> &other) const { return other.Ptr == Ptr; }

  bool operator!=(const NullPtr<T> &other) const { return !(*this == other); }

  bool operator==(const T *other) const { return other == Ptr; }

  bool operator!=(const T *other) const { return !(*this == other); }
};

} // end namespace stone

namespace llvm {
template <typename T> struct PointerLikeTypeTraits;
template <typename T> struct PointerLikeTypeTraits<stone::NullPtr<T>> {
public:
  static inline void *getAsVoidPointer(stone::NullPtr<T> ptr) {
    return static_cast<void *>(ptr.getPtrOrNull());
  }
  static inline stone::NullPtr<T> getFromVoidPointer(void *ptr) {
    return stone::NullPtr<T>(static_cast<T *>(ptr));
  }
  enum {
    NumLowBitsAvailable = PointerLikeTypeTraits<T *>::NumLowBitsAvailable
  };
};

} // namespace llvm

#endif