#ifndef LLVM_CLANG_CORE_IDENTIFIERTABLE_H
#define LLVM_CLANG_CORE_IDENTIFIERTABLE_H

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>
#include <utility>

#include "stone/Core/EditorPlaceholder.h"
#include "stone/Core/LLVM.h"
#include "stone/Core/StatisticEngine.h"
#include "stone/Core/TokenKind.h"
#include "llvm/ADT/DenseMapInfo.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Allocator.h"
#include "llvm/Support/PointerLikeTypeTraits.h"
#include "llvm/Support/type_traits.h"

namespace stone {
class SystemOptions;
class SrcLoc;
namespace syn {

class DeclName;
class DeclNameTable;
class Identifier;
/// A simple pair of identifier info and location.
using IdentifierLocPair = std::pair<Identifier *, SrcLoc>;

/// Identifier and other related classes are aligned to
/// 8 bytes so that DeclName can use the lower 3 bits
/// of a pointer to one of these classes.
enum { IdentifierAlignment = 8 };

/// One of these records is kept for each identifier that
/// is lexed.  This contains information about whether the token was \#define'd,
/// is a language keyword, or if it is a front-end token of some sort (e.g. a
/// variable or function name).  The preprocessor keeps this information in a
/// set, and all tok::identifier tokens have a pointer to one of these.
/// It is aligned to 8 bytes because DeclName needs the lower 3 bits.
class alignas(IdentifierAlignment) Identifier {
  friend class SyntaxContext;
  friend class IdentifierTable; // TODO: Replace with SyntaxContext

  // Front-end token ID or tok::identifier.
  tok ty;

  unsigned BuiltinID;

  // True if the identifier is a keyword in a newer or proposed Standard.
  unsigned isKeywordReserved : 1;

  // True if the identifier is a C++ operator keyword.
  unsigned IsOperatorKeyword : 1;

  llvm::StringMapEntry<Identifier *> *entry = nullptr;

public:
  explicit Identifier()
      : ty(tok::identifier), BuiltinID(0), isKeywordReserved(false),
        IsOperatorKeyword(false) {}

  /// Return true if this is the identifier for the specified string.
  ///
  /// This is intended to be used for string literals only: II->isStr("foo").
  template <std::size_t StrLen> bool isStr(const char (&Str)[StrLen]) const {
    return getLength() == StrLen - 1 &&
           memcmp(getNameStart(), Str, StrLen - 1) == 0;
  }

  /// Return true if this is the identifier for the specified StringRef.
  bool isStr(llvm::StringRef Str) const {
    llvm::StringRef ThisStr(getNameStart(), getLength());
    return ThisStr == Str;
  }

  /// Return the beginning of the actual null-terminated string for this
  /// identifier.
  const char *getNameStart() const { return entry->getKeyData(); }

  /// Efficiently return the length of this identifier info.
  unsigned getLength() const { return entry->getKeyLength(); }

  /// Return the actual identifier string.
  llvm::StringRef GetName() const {
    return StringRef(getNameStart(), getLength());
  }

  bool IsArithmeticOperator() const {
    return isStr("+") || isStr("-") || isStr("*") || isStr("/") || isStr("%");
  }

  // Returns whether this is a standard comparison operator,
  // such as '==', '>=' or '!=='.
  bool IsSTDComparisonOperator() const {
    return isStr("==") || isStr("!=") || isStr("===") || isStr("!==") ||
           isStr("<") || isStr(">") || isStr("<=") || isStr(">=");
  }

  /// isOperatorStartCodePoint - Return true if the specified code point is a
  /// valid start of an operator.
  static bool IsOperatorStartCodePoint(uint32_t C) {
    // ASCII operator chars.
    static const char OpChars[] = "/=-+*%<>!&|^~.?";
    if (C < 0x80)
      return memchr(OpChars, C, sizeof(OpChars) - 1) != 0;

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
    if (IsOperatorStartCodePoint(C))
      return true;

    // Unicode combining characters and variation selectors.
    return (C >= 0x0300 && C <= 0x036F) || (C >= 0x1DC0 && C <= 0x1DFF) ||
           (C >= 0x20D0 && C <= 0x20FF) || (C >= 0xFE00 && C <= 0xFE0F) ||
           (C >= 0xFE20 && C <= 0xFE2F) || (C >= 0xE0100 && C <= 0xE01EF);
  }

  static bool IsEditorPlaceholder(StringRef name) {
    return stone::isEditorPlaceholder(name);
  }

  /// If this is a source-language token (e.g. 'for'), this API
  /// can be used to cause the lexer to map identifiers to source-language
  /// tokens.
  tok GetTokenType() const { return ty; }

  /// Return a value indicating whether this is a builtin function.
  ///
  /// 0 is not-built-in. 1+ are specific builtin functions.
  unsigned GetBuiltinID() const { return BuiltinID; }
  void SetBuiltinID(unsigned ID) { BuiltinID = ID; }

  /// is/setIsKeywordReserved - Initialize information about whether or not
  /// this language token is a keyword in a newer or proposed Standard. This
  /// controls compatibility warnings, and is only true when not parsing the
  /// corresponding Standard. Once a compatibility problem has been diagnosed
  /// with this keyword, the flag will be cleared.
  bool IsKeywordReserved() const { return isKeywordReserved; }

  void SetIsKeywordReserved(bool reserved) { isKeywordReserved = reserved; }

  /// isCPlusPlusOperatorKeyword/setIsCPlusPlusOperatorKeyword controls whether
  /// this identifier is a C++ alternate representation of an operator.
  void setIsOperatorKeyword(bool Val = true) { IsOperatorKeyword = Val; }
  bool isOperatorKeyword() const { return IsOperatorKeyword; }

  /// Return true if this token is a keyword in the specified language.
  bool IsKeyword(const SystemOptions &LangOpts) const;

  /// Return true if this identifier is an editor placeholder.
  ///
  /// Editor placeholders are produced by the code-completion engine and are
  /// represented as characters between '<#' and '#>' in the source code. An
  /// example of auto-completed call with a placeholder parameter is shown
  /// below:
  /// \code
  ///   function(<#int x#>);
  /// \endcode
  bool isEditorPlaceholder() const {
    return GetName().startswith("<#") && GetName().endswith("#>");
  }

  /// Provide less than operator for lexicographical sorting.
  bool operator<(const Identifier &RHS) const {
    return GetName() < RHS.GetName();
  }

  bool static IsIdentifier(llvm::StringRef identifier);
};

/// An iterator that walks over all of the known identifiers
/// in the lookup table.
///
/// Since this iterator uses an abstract interface via virtual
/// functions, it uses an object-oriented interface rather than the
/// more standard C++ STL iterator interface. In this OO-style
/// iteration, the single function \c Next() provides dereference,
/// advance, and end-of-sequence checking in a single
/// operation. Subclasses of this iterator type will provide the
/// actual functionality.
class IdentifierIterator {
protected:
  IdentifierIterator() = default;

public:
  IdentifierIterator(const IdentifierIterator &) = delete;
  IdentifierIterator &operator=(const IdentifierIterator &) = delete;

  virtual ~IdentifierIterator();

  /// Retrieve the next string in the identifier table and
  /// advances the iterator for the following string.
  ///
  /// \returns The next string in the identifier table. If there is
  /// no such string, returns an empty \c StringRef.
  virtual StringRef Next() = 0;
};

class IdentifierTable;
class IdentifierTableStats final : public Stats {
  const IdentifierTable &table;

public:
  IdentifierTableStats(const IdentifierTable &table)
      : Stats("identifier table stats:"), table(table) {}
  void Print(ColorfulStream &stream) override;
};

/// Implements an efficient mapping from strings to Identifier nodes.
///
/// This has no other purpose, but this is an extremely performance-critical
/// piece of the code, as each occurrence of every identifier goes through
/// here when lexed.
class IdentifierTable final {
  const SystemOptions &systemOpts;
  friend IdentifierTableStats;

  using Symbols = llvm::StringMap<Identifier *, llvm::BumpPtrAllocator>;
  Symbols symbols;

public:
  /// Create the identifier table, populating it with info about the
  /// language keywords for the language specified by \p LangOpts.
  explicit IdentifierTable(const SystemOptions &systemOpts);

  llvm::BumpPtrAllocator &GetAllocator() { return symbols.getAllocator(); }

  /// Return the identifier token info for the specified named
  /// identifier.
  Identifier &Get(llvm::StringRef name) {
    auto &entry = *symbols.insert(std::make_pair(name, nullptr)).first;
    Identifier *&identifier = entry.second;
    if (identifier) {
      return *identifier;
    }
    // Lookups failed, make a new Identifier.
    void *mem = GetAllocator().Allocate<Identifier>();
    identifier = new (mem) Identifier();

    // Make sure GetName() knows how to find the Identifier
    // contents.
    identifier->entry = &entry;
    return *identifier;
  }

  Identifier &Get(llvm::StringRef name, tok t) {
    auto &identifier = Get(name);
    identifier.ty = t;
    assert(identifier.ty == t && "TokenCode too large");
    return identifier;
  }

  /// Gets an Identifier for the given name without consulting
  ///        external sources.
  ///
  /// This is a version of Get() meant for external sources that want to
  /// introduce or modify an identifier. If they called Get(), they would
  /// likely end up in a recursion.
  Identifier &GetOwn(llvm::StringRef name) {
    auto &entry = *symbols.insert(std::make_pair(name, nullptr)).first;
    Identifier *&identifier = entry.second;
    if (identifier) {
      return *identifier;
    }
    // Lookups failed, make a new Identifier.
    void *mem = GetAllocator().Allocate<Identifier>();
    identifier = new (mem) Identifier();

    // Make sure GetName() knows how to find the Identifier
    // contents.
    identifier->entry = &entry;
    return *identifier;
  }

  using iterator = Symbols::const_iterator;
  using const_iterator = Symbols::const_iterator;

  iterator begin() const { return symbols.begin(); }
  iterator end() const { return symbols.end(); }
  unsigned size() const { return symbols.size(); }

  /// Populate the identifier table with info about the language keywords
  /// for the language specified by \p LangOpts.
  void AddKeywords(const SystemOptions &LangOpts);
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
class alignas(IdentifierAlignment) SpecialDeclName {
  friend class stone::syn::DeclName;
  friend class stone::syn::DeclNameTable;

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
} // namespace syn
} // namespace stone

namespace llvm {
// Provide PointerLikeTypeTraits for Identifier pointers, which
// are not guaranteed to be 8-byte aligned.
template <> struct PointerLikeTypeTraits<stone::syn::Identifier *> {
  static void *getAsVoidPointer(stone::syn::Identifier *P) { return P; }

  static stone::syn::Identifier *getFromVoidPointer(void *P) {
    return static_cast<stone::syn::Identifier *>(P);
  }

  enum { NumLowBitsAvailable = 1 };
};

template <> struct PointerLikeTypeTraits<const stone::syn::Identifier *> {
  static const void *getAsVoidPointer(const stone::syn::Identifier *P) {
    return P;
  }

  static const stone::syn::Identifier *getFromVoidPointer(const void *P) {
    return static_cast<const stone::syn::Identifier *>(P);
  }

  enum { NumLowBitsAvailable = 1 };
};

} // namespace llvm

#endif
