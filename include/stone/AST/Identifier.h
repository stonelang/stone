#ifndef STONE_AST_IDENTIFIER_H
#define STONE_AST_IDENTIFIER_H

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>
#include <utility>

#include "stone/Basic/EditorPlaceholder.h"
#include "stone/Basic/LLVM.h"
#include "stone/Basic/TokenKind.h"
#include "stone/Support/Statistics.h"

#include "llvm/ADT/DenseMapInfo.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Allocator.h"
#include "llvm/Support/PointerLikeTypeTraits.h"
#include "llvm/Support/type_traits.h"

namespace stone {
class SrcLoc;
class LangOptions;

class DeclName;
class DeclNameTable;
class Identifier;
class ASTContext;

/// One of these records is kept for each identifier that
/// is lexed.  This contains information about whether the token was \#define'd,
/// is a language keyword, or if it is a front-end token of some sort (e.g. a
/// variable or function name).  The preprocessor keeps this information in a
/// set, and all tok::identifier tokens have a pointer to one of these.
/// It is aligned to 8 bytes because DeclName needs the lower 3 bits.
class Identifier final {
  friend class ASTContext;
  friend class DeclNameBase;

  const char *dataPointer;

public:
  enum : size_t {
    NumLowBitsAvailable = 3,
    RequiredAlignment = 1 << NumLowBitsAvailable,
    SpareBitMask = ((intptr_t)1 << NumLowBitsAvailable) - 1
  };

  /// A type with the alignment expected of a valid \c Identifier::Pointer .
  struct alignas(uint64_t) Aligner {};

  static_assert(alignof(Aligner) >= RequiredAlignment,
                "Identifier table will provide enough spare bits");

private:
  explicit Identifier(const char *dataPtr) : dataPointer(dataPtr) {}

public:
  explicit Identifier() : dataPointer(nullptr) {}

  const char *GetPointer() const { return dataPointer; }

  llvm::StringRef GetString() const { return dataPointer; }
  explicit operator std::string() const { return std::string(dataPointer); }

  unsigned GetLength() const {
    assert(dataPointer != nullptr &&
           "Tried getting length of empty identifier");
    return ::strlen(dataPointer);
  }
  bool IsEmpty() const { return dataPointer == nullptr; }
  bool IsEqual(llvm::StringRef other) const {
    return GetString().equals(other);
  }

  // bool IsOperator() const {
  //   if (IsEmpty()) {
  //     return false;
  //   }
  //   if (IsEditorPlaceholder()) {
  //     return false;
  //   }
  //   if ((unsigned char)dataPointer[0] < 0x80) {
  //     return IsOperatorStartCodePoint((unsigned char)dataPointer[0]);
  //   }
  //   // Handle the high unicode case out of line.
  //   return IsOperatorSlow();
  // }

  bool IsOperatorSlow() const;

  /// Return true if this identifier is specified by the language.
  bool IsKeyword() const {
    assert(false && "Not implemented");
    return true;
  }
  // bool IsBuiltin() const;

  bool IsArithmeticOperator() const {
    return IsEqual("+") || IsEqual("-") || IsEqual("*") || IsEqual("/") ||
           IsEqual("%");
  }
  // Returns whether this is a standard comparison operator,
  // such as '==', '>=' or '!=='.
  bool IsSTDComparisonOperator() const {
    return IsEqual("==") || IsEqual("!=") || IsEqual("===") || IsEqual("!==") ||
           IsEqual("<") || IsEqual(">") || IsEqual("<=") || IsEqual(">=");
  }

  /// isOperatorStartCodePoint - Return true if the specified code point is a
  /// valid start of an operator.
  static bool IsOperatorStartCodePoint(uint32_t C) {
    // ASCII operator chars.
    static const char OpChars[] = "/=-+*%<>!&|^~.?";
    if (C < 0x80) {
      return memchr(OpChars, C, sizeof(OpChars) - 1) != 0;
    }

    // Unicode math, symbol, arrow, dingbat, and line/box drawing chars.
    return (C >= 0x00A1 && C <= 0x00A7) || C == 0x00A9 || C == 0x00AB ||
           C == 0x00AC || C == 0x00AE || C == 0x00B0 || C == 0x00B1 ||
           C == 0x00B6 || C == 0x00BB || C == 0x00BF || C == 0x00D7 ||
           C == 0x00F7 || C == 0x2016 || C == 0x2017 ||
           (C >= 0x2020 && C <= 0x2027) || (C >= 0x2030 && C <= 0x203E) ||
           (C >= 0x2041 && C <= 0x2053) || (C >= 0x2055 && C <= 0x205E) ||
           (C >= 0x2190 && C <= 0x23FF) || (C >= 0x2500 && C <= 0x2775) ||
           (C >= 0x2794 && C <= 0x2BFF) || (C >= 0x2E00 && C <= 0x2E7F) ||
           (C >= 0x3001 && C <= 0x3003) || (C >= 0x3008 && C <= 0x3030);
  }

  /// isOperatorContinuationCodePoint - Return true if the specified code point
  /// is a valid operator code point.
  static bool IsOperatorContinuationCodePoint(uint32_t C) {
    if (IsOperatorStartCodePoint(C)) {
      return true;
    }
    // Unicode combining characters and variation selectors.
    return (C >= 0x0300 && C <= 0x036F) || (C >= 0x1DC0 && C <= 0x1DFF) ||
           (C >= 0x20D0 && C <= 0x20FF) || (C >= 0xFE00 && C <= 0xFE0F) ||
           (C >= 0xFE20 && C <= 0xFE2F) || (C >= 0xE0100 && C <= 0xE01EF);
  }

  static bool IsEditorPlaceholder(llvm::StringRef name) {
    return stone::isEditorPlaceholder(name);
  }

  bool IsEditorPlaceholder() const {
    return !IsEmpty() && isEditorPlaceholder(GetString());
  }

  const void *GetAsOpaquePointer() const {
    return static_cast<const void *>(dataPointer);
  }

  static Identifier GetFromOpaquePointer(void *Ptr) {
    return Identifier((const char *)Ptr);
  }

  /// Compare two identifiers, producing -1 if \c *this comes before \c other,
  /// 1 if \c *this comes after \c other, and 0 if they are equal.
  ///
  /// Null identifiers come after all other identifiers.
  int Compare(Identifier other) const;

  friend llvm::hash_code hash_value(Identifier identifier) {
    return llvm::hash_value(identifier.GetAsOpaquePointer());
  }

  bool operator==(Identifier RHS) const {
    return dataPointer == RHS.dataPointer;
  }
  bool operator!=(Identifier RHS) const { return !(*this == RHS); }
  bool operator<(Identifier RHS) const { return dataPointer < RHS.dataPointer; }

public:
  static Identifier getEmptyKey() {
    uintptr_t Val = static_cast<uintptr_t>(-1);
    Val <<= NumLowBitsAvailable;
    return Identifier((const char *)Val);
  }

  static Identifier getTombstoneKey() {
    uintptr_t Val = static_cast<uintptr_t>(-2);
    Val <<= NumLowBitsAvailable;
    return Identifier((const char *)Val);
  }
};

namespace detail {
/// SpecialDeclName is used as a base of various uncommon special names.
/// This class is needed since DeclName has not enough space to store
/// the kind of every possible names. Therefore the kind of common names is
/// stored directly in DeclName, and the kind of uncommon names is
/// stored in DeclNameExtra. It is aligned to 8 bytes because
/// DeclName needs the lower 3 bits to store the kind of common names.
/// DeclNameExtra is tightly coupled to DeclName and any change
/// here is very likely to require changes in DeclName(Table).
class SpecialDeclName {
  friend class stone::DeclName;
  friend class stone::DeclNameTable;

protected:
  /// The kind of "extra" information stored in the DeclName. See
  /// @c ExtraKindOrNumArgs for an explanation of how these enumerator values
  /// are used. Note that DeclName depends on the numerical values
  /// of the enumerators in this enum. See DeclName::StoredNameKind
  /// for more info.
  enum SpecialKind {
    DeductionGuideName,
    LiteralOperatorName,
    UsingDirective,
  };

  /// ExtraKindOrNumArgs has one of the following meaning:
  ///  * The kind of an uncommon/special name. This SpecialDeclName
  ///    is in this case in fact either a DeductionGuideNameExtra or
  ///    a LiteralOperatorIdName.
  ///
  ///  * It may be also name common to using-directives (UsingDirective),
  SpecialKind kind;

  SpecialDeclName(SpecialKind kind) : kind(kind) {}

  /// Return the corresponding SpecialKind.
  SpecialKind GetKind() const { return kind; }
};

} // namespace detail
} // namespace stone

namespace llvm {

raw_ostream &operator<<(raw_ostream &OS, stone::Identifier I);
// Identifiers hash just like pointers.
template <> struct DenseMapInfo<stone::Identifier> {
  static stone::Identifier getEmptyKey() {
    return stone::Identifier::getEmptyKey();
  }
  static stone::Identifier getTombstoneKey() {
    return stone::Identifier::getTombstoneKey();
  }
  static unsigned getHashValue(stone::Identifier Val) {
    return DenseMapInfo<const void *>::getHashValue(Val.GetPointer());
  }
  static bool isEqual(stone::Identifier LHS, stone::Identifier RHS) {
    return LHS == RHS;
  }
};

// An Identifier is "pointer like".
template <typename T> struct PointerLikeTypeTraits;
template <> struct PointerLikeTypeTraits<stone::Identifier> {
public:
  static inline void *getAsVoidPointer(stone::Identifier I) {
    return const_cast<void *>(I.GetAsOpaquePointer());
  }
  static inline stone::Identifier getFromVoidPointer(void *P) {
    return stone::Identifier::GetFromOpaquePointer(P);
  }
  enum { NumLowBitsAvailable = stone::Identifier::NumLowBitsAvailable };
};

} // namespace llvm

#endif
