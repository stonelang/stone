
//===----------------------------------------------------------------------===//
//
//  This file defines the Redeclarable interface.
//
//===----------------------------------------------------------------------===//

#ifndef STONE_AST_REDECLARABLE_H
#define STONE_AST_REDECLARABLE_H

#include "llvm/ADT/DenseMapInfo.h"
#include "llvm/ADT/PointerUnion.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/Support/Casting.h"

#include <cassert>
#include <cstddef>
#include <iterator>

namespace stone {

class Decl;
class ASTContext;

// Some notes on redeclarables:
//
//  - Every redeclarable is on a circular linked list.
//
//  - Every decl has a pointer to the first element of the chain _and_ a
//    DeclLink that may point to one of 3 possible states:
//      - the "previous" (temporal) element in the chain
//      - the "latest" (temporal) element in the chain
//      - the "uninitialized-latest" value (when newly-constructed)
//
//  - The first element is also often called the canonical element. Every
//    element has a pointer to it so that "getCanonical" can be fast.
//
//  - Most links in the chain point to previous, except the link out of
//    the first; it points to latest.
//
//  - Elements are called "first", "previous", "latest" or
//    "most-recent" when referring to temporal order: order of addition
//    to the chain.
//
//  - It's easiest to just ignore the implementation of DeclLink when making
//    sense of the redeclaration chain.
//
//  - There's also a "definition" link for several types of
//    redeclarable, where only one definition should exist at any given
//    time (and the defn pointer is stored in the decl's "data" which
//    is copied to every element on the chain when it's changed).
//
//    Here is some ASCII art:
//
//      "first"                                     "latest"
//      "canonical"                                 "most recent"
//      +------------+         first                +--------------+
//      |            | <--------------------------- |              |
//      |            |                              |              |
//      |            |                              |              |
//      |            |       +--------------+       |              |
//      |            | first |              |       |              |
//      |            | <---- |              |       |              |
//      |            |       |              |       |              |
//      | @class A   |  link | @interface A |  link | @class A     |
//      | seen first | <---- | seen second  | <---- | seen third   |
//      |            |       |              |       |              |
//      +------------+       +--------------+       +--------------+
//      | data       | defn  | data         |  defn | data         |
//      |            | ----> |              | <---- |              |
//      +------------+       +--------------+       +--------------+
//        |                     |     ^                  ^
//        |                     |defn |                  |
//        | link                +-----+                  |
//        +-->-------------------------------------------+

/// Provides common interface for the Decls that can be redeclared.
template <typename DeclTy> class Redeclarable {
protected:
  class DeclLink {
    /// A pointer to a known latest declaration, either statically known or
    /// generationally updated as decls are added by an external source.
    using KnownLatest = LazyGenerationalUpdatePtr<const Decl *, Decl *>;

    /// We store a pointer to the ASTContext in the UninitializedLatest
    /// pointer, but to avoid circular type dependencies when we steal the low
    /// bits of this pointer, we use a raw void* here.
    using UninitializedLatest = const void *;

    using Previous = Decl *;

    /// A pointer to either an uninitialized latest declaration (where either
    /// we've not yet set the previous decl or there isn't one), or to a known
    /// previous declaration.
    using NotKnownLatest = llvm::PointerUnion<Previous, UninitializedLatest>;

    mutable llvm::PointerUnion<NotKnownLatest, KnownLatest> Link;

  public:
    enum PreviousTag { PreviousLink };
    enum LatestTag { LatestLink };

    DeclLink(LatestTag, const ASTContext &Ctx)
        : Link(NotKnownLatest(reinterpret_cast<UninitializedLatest>(&Ctx))) {}
    DeclLink(PreviousTag, DeclTy *D) : Link(NotKnownLatest(Previous(D))) {}

    bool isFirst() const {
      return Link.is<KnownLatest>() ||
             // FIXME: 'template' is required on the next line due to an
             // apparent stone bug.
             Link.get<NotKnownLatest>().template is<UninitializedLatest>();
    }

    DeclTy *getPrevious(const DeclTy *D) const {
      if (Link.is<NotKnownLatest>()) {
        NotKnownLatest NKL = Link.get<NotKnownLatest>();
        if (NKL.is<Previous>())
          return static_cast<DeclTy *>(NKL.get<Previous>());

        // Allocate the generational 'most recent' cache now, if needed.
        Link = KnownLatest(*reinterpret_cast<const ASTContext *>(
                               NKL.get<UninitializedLatest>()),
                           const_cast<DeclTy *>(D));
      }

      return static_cast<DeclTy *>(Link.get<KnownLatest>().get(D));
    }

    void setPrevious(DeclTy *D) {
      assert(!isFirst() && "decl became non-canonical unexpectedly");
      Link = Previous(D);
    }

    void setLatest(DeclTy *D) {
      assert(isFirst() && "decl became canonical unexpectedly");
      if (Link.is<NotKnownLatest>()) {
        NotKnownLatest NKL = Link.get<NotKnownLatest>();
        Link = KnownLatest(*reinterpret_cast<const ASTContext *>(
                               NKL.get<UninitializedLatest>()),
                           D);
      } else {
        auto Latest = Link.get<KnownLatest>();
        Latest.set(D);
        Link = Latest;
      }
    }

    void markIncomplete() { Link.get<KnownLatest>().markIncomplete(); }

    Decl *getLatestNotUpdated() const {
      assert(isFirst() && "expected a canonical decl");
      if (Link.is<NotKnownLatest>())
        return nullptr;
      return Link.get<KnownLatest>().getNotUpdated();
    }
  };

  static DeclLink PreviousDeclLink(DeclTy *D) {
    return DeclLink(DeclLink::PreviousLink, D);
  }

  static DeclLink LatestDeclLink(const ASTContext &Ctx) {
    return DeclLink(DeclLink::LatestLink, Ctx);
  }

  /// Points to the next redeclaration in the chain.
  ///
  /// If isFirst() is false, this is a link to the previous declaration
  /// of this same Decl. If isFirst() is true, this is the first
  /// declaration and Link points to the latest declaration. For example:
  ///
  ///  #1 int f(int x, int y = 1); // <pointer to #3, true>
  ///  #2 int f(int x = 0, int y); // <pointer to #1, false>
  ///  #3 int f(int x, int y) { return x + y; } // <pointer to #2, false>
  ///
  /// If there is only one declaration, it is <pointer to self, true>
  DeclLink RedeclLink;

  DeclTy *First;

  DeclTy *getNextRedeclaration() const {
    return RedeclLink.getPrevious(static_cast<const DeclTy *>(this));
  }

public:
  Redeclarable(const ASTContext &Ctx)
      : RedeclLink(LatestDeclLink(Ctx)), First(static_cast<DeclTy *>(this)) {}

  /// Return the previous declaration of this declaration or NULL if this
  /// is the first declaration.
  DeclTy *getPreviousDecl() {
    if (!RedeclLink.isFirst())
      return getNextRedeclaration();
    return nullptr;
  }
  const DeclTy *getPreviousDecl() const {
    return const_cast<DeclTy *>(static_cast<const DeclTy *>(this))
        ->getPreviousDecl();
  }

  /// Return the first declaration of this declaration or itself if this
  /// is the only declaration.
  DeclTy *getFirstDecl() { return First; }

  /// Return the first declaration of this declaration or itself if this
  /// is the only declaration.
  const DeclTy *getFirstDecl() const { return First; }

  /// True if this is the first declaration in its redeclaration chain.
  bool isFirstDecl() const { return RedeclLink.isFirst(); }

  /// Returns the most recent (re)declaration of this declaration.
  DeclTy *getMostRecentDecl() { return getFirstDecl()->getNextRedeclaration(); }

  /// Returns the most recent (re)declaration of this declaration.
  const DeclTy *getMostRecentDecl() const {
    return getFirstDecl()->getNextRedeclaration();
  }

  /// Set the previous declaration. If PrevDecl is NULL, set this as the
  /// first and only declaration.
  void setPreviousDecl(DeclTy *PrevDecl);

  /// Iterates through all the redeclarations of the same decl.
  class redecl_iterator {
    /// Current - The current declaration.
    DeclTy *Current = nullptr;
    DeclTy *Starter;
    bool PassedFirst = false;

  public:
    using value_type = DeclTy *;
    using reference = DeclTy *;
    using pointer = DeclTy *;
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;

    redecl_iterator() = default;
    explicit redecl_iterator(DeclTy *C) : Current(C), Starter(C) {}

    reference operator*() const { return Current; }
    pointer operator->() const { return Current; }

    redecl_iterator &operator++() {
      assert(Current && "Advancing while iterator has reached end");
      // Make sure we don't infinitely loop on an invalid redecl chain. This
      // should never happen.
      if (Current->isFirstDecl()) {
        if (PassedFirst) {
          assert(0 && "Passed first decl twice, invalid redecl chain!");
          Current = nullptr;
          return *this;
        }
        PassedFirst = true;
      }

      // Get either previous decl or latest decl.
      DeclTy *Next = Current->getNextRedeclaration();
      Current = (Next != Starter) ? Next : nullptr;
      return *this;
    }

    redecl_iterator operator++(int) {
      redecl_iterator tmp(*this);
      ++(*this);
      return tmp;
    }

    friend bool operator==(redecl_iterator x, redecl_iterator y) {
      return x.Current == y.Current;
    }
    friend bool operator!=(redecl_iterator x, redecl_iterator y) {
      return x.Current != y.Current;
    }
  };

  using redecl_range = llvm::iterator_range<redecl_iterator>;

  /// Returns an iterator range for all the redeclarations of the same
  /// decl. It will iterate at least once (when this decl is the only one).
  redecl_range redecls() const {
    return redecl_range(redecl_iterator(const_cast<DeclTy *>(
                            static_cast<const DeclTy *>(this))),
                        redecl_iterator());
  }

  redecl_iterator redecls_begin() const { return redecls().begin(); }
  redecl_iterator redecls_end() const { return redecls().end(); }
};

/// Get the primary declaration for a declaration from an AST file. That
/// will be the first-loaded declaration.
Decl *getPrimaryMergedDecl(Decl *D);

/// Provides common interface for the Decls that cannot be redeclared,
/// but can be merged if the same declaration is brought in from multiple
/// modules.
template <typename DeclTy> class Mergeable {
public:
  Mergeable() = default;

  /// Return the first declaration of this declaration or itself if this
  /// is the only declaration.
  DeclTy *getFirstDecl() {
    auto *D = static_cast<DeclTy *>(this);
    return D;
  }

  /// Return the first declaration of this declaration or itself if this
  /// is the only declaration.
  const DeclTy *getFirstDecl() const {
    const auto *D = static_cast<const DeclTy *>(this);
    return D;
  }

  /// Returns true if this is the first declaration.
  bool isFirstDecl() const { return getFirstDecl() == this; }
};

/// A wrapper class around a pointer that always points to its canonical
/// declaration.
///
/// CanonicalDeclPtr<DeclTy> behaves just like DeclTy*, except we call
/// DeclTy::getCanonicalDecl() on construction.
///
/// This is useful for hashtables that you want to be keyed on a declaration's
/// canonical decl -- if you use CanonicalDeclPtr as the key, you don't need to
/// remember to call getCanonicalDecl() everywhere.
template <typename DeclTy> class CanonicalDeclPtr {
public:
  CanonicalDeclPtr() = default;
  CanonicalDeclPtr(DeclTy *Ptr)
      : Ptr(Ptr ? Ptr->getCanonicalDecl() : nullptr) {}
  CanonicalDeclPtr(const CanonicalDeclPtr &) = default;
  CanonicalDeclPtr &operator=(const CanonicalDeclPtr &) = default;

  operator DeclTy *() { return Ptr; }
  operator const DeclTy *() const { return Ptr; }

  DeclTy *operator->() { return Ptr; }
  const DeclTy *operator->() const { return Ptr; }

  DeclTy &operator*() { return *Ptr; }
  const DeclTy &operator*() const { return *Ptr; }

  friend bool operator==(CanonicalDeclPtr LHS, CanonicalDeclPtr RHS) {
    return LHS.Ptr == RHS.Ptr;
  }
  friend bool operator!=(CanonicalDeclPtr LHS, CanonicalDeclPtr RHS) {
    return LHS.Ptr != RHS.Ptr;
  }

private:
  friend struct llvm::DenseMapInfo<CanonicalDeclPtr<DeclTy>>;
  friend struct llvm::PointerLikeTypeTraits<CanonicalDeclPtr<DeclTy>>;

  DeclTy *Ptr = nullptr;
};
} // namespace stone

namespace llvm {

template <typename DeclTy>
struct DenseMapInfo<stone::CanonicalDeclPtr<DeclTy>> {
  using CanonicalDeclPtr = stone::CanonicalDeclPtr<DeclTy>;
  using BaseInfo = DenseMapInfo<DeclTy *>;

  static CanonicalDeclPtr getEmptyKey() {
    // Construct our CanonicalDeclPtr this way because the regular constructor
    // would dereference P.Ptr, which is not allowed.
    CanonicalDeclPtr P;
    P.Ptr = BaseInfo::getEmptyKey();
    return P;
  }

  static CanonicalDeclPtr getTombstoneKey() {
    CanonicalDeclPtr P;
    P.Ptr = BaseInfo::getTombstoneKey();
    return P;
  }

  static unsigned getHashValue(const CanonicalDeclPtr &P) {
    return BaseInfo::getHashValue(P);
  }

  static bool isEqual(const CanonicalDeclPtr &LHS,
                      const CanonicalDeclPtr &RHS) {
    return BaseInfo::isEqual(LHS, RHS);
  }
};

template <typename DeclTy>
struct PointerLikeTypeTraits<stone::CanonicalDeclPtr<DeclTy>> {
  static inline void *getAsVoidPointer(stone::CanonicalDeclPtr<DeclTy> P) {
    return P.Ptr;
  }
  static inline stone::CanonicalDeclPtr<DeclTy> getFromVoidPointer(void *P) {
    stone::CanonicalDeclPtr<DeclTy> C;
    C.Ptr = PointerLikeTypeTraits<DeclTy *>::getFromVoidPtr(P);
    return C;
  }
  static constexpr int NumLowBitsAvailable =
      PointerLikeTypeTraits<DeclTy *>::NumLowBitsAvailable;
};

} // namespace llvm

#endif // STONE_AST_REDECLARABLE_H
