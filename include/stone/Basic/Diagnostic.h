#ifndef STONE_BASIC_DIAGNOSTIC_H
#define STONE_BASIC_DIAGNOSTIC_H

#include "stone/Basic/DiagnosticArgument.h"
#include "stone/Basic/SrcLoc.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/IntrusiveRefCntPtr.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/Error.h"

#include <cassert>
#include <cstdint>
#include <limits>
#include <list>
#include <map>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace stone {

/// Enumeration describing all of possible diagnostics.
///
/// Each of the diagnostics described in DiagnosticEngine.def has an entry in
/// this enumeration type that uniquely identifies it.
enum class DiagID : uint32_t;

enum class FixID : uint32_t;

/// Describes a diagnostic along with its argument types.
///
/// The diagnostics header introduces instances of this type for each
/// diagnostic, which provide both the set of argument types (used to
/// check/convert the arguments at each call site) and the diagnostic ID
/// (for other information about the diagnostic).
template <typename... argTypes> struct Diag {
  /// The diagnostic ID corresponding to this diagnostic.
  DiagID diagID;
};

namespace detail {
/// Describes how to pass a diagnostic argument of the given type.
///
/// By default, diagnostic arguments are passed by value, because they
/// tend to be small. Larger diagnostic arguments
/// need to specialize this class template to pass by reference.
template <typename T> struct PassArgument { typedef T type; };
} // namespace detail

class FixHint final {
public:
  /// Code that should be replaced to correct the error. Empty for an
  /// insertion hint.
  CharSrcRange removeRange;

  /// Code in the specific range that should be inserted in the insertion
  /// location.
  CharSrcRange insertFromRange;

  /// The actual code to insert at the insertion location, as a
  /// string.
  std::string codeToInsert;

  bool beforePreviousInsertions = false;

  /// Empty code modification hint, indicating that no code
  /// modification is known.
  FixHint() = default;

  bool IsNull() const { return !removeRange.isValid(); }

  /// Create a code modification hint that inserts the given
  /// code string at a specific location.
  static FixHint CreateInsertion(SrcLoc insertionLoc, StringRef code,
                                 bool beforePreviousInsertions = false) {
    FixHint fix;
    fix.removeRange = CharSrcRange::getCharRange(insertionLoc, insertionLoc);
    fix.codeToInsert = std::string(code);
    fix.beforePreviousInsertions = beforePreviousInsertions;
    return fix;
  }

  /// Create a code modification hint that inserts the given
  /// code from \p FromRange at a specific location.
  static FixHint
  CreateInsertionFromRange(SrcLoc insertionLoc, CharSrcRange fromRange,
                           bool beforePreviousInsertions = false) {
    FixHint fix;
    fix.removeRange = CharSrcRange::getCharRange(insertionLoc, insertionLoc);
    fix.insertFromRange = fromRange;
    fix.beforePreviousInsertions = beforePreviousInsertions;
    return fix;
  }

  /// Create a code modification hint that removes the given
  /// source range.
  static FixHint CreateRemoval(CharSrcRange removeRange) {
    FixHint fix;
    fix.removeRange = removeRange;
    return fix;
  }
  static FixHint CreateRemoval(SrcRange removeRange) {
    return CreateRemoval(CharSrcRange::getTokenRange(removeRange));
  }

  /// Create a code modification hint that replaces the given
  /// source range with the given code string.
  static FixHint CreateReplacement(CharSrcRange removeRange,
                                   llvm::StringRef code) {
    FixHint fix;
    fix.removeRange = removeRange;
    fix.codeToInsert = std::string(code);
    return fix;
  }

  static FixHint CreateReplacement(SrcRange removeRange, llvm::StringRef code) {
    return CreateReplacement(CharSrcRange::getTokenRange(removeRange), code);
  }
};
struct DiagnosticFormatOptions final {};

/// Pass the D
class DiagnosticContext final {
  DiagID diagID;
  llvm::SmallVector<DiagnosticArgument, 3> args;
  llvm::SmallVector<CharSrcRange, 2> ranges;
  llvm::SmallVector<FixHint, 2> hints;

public:
  template <typename... ArgTypes>
  DiagnosticContext(Diag<ArgTypes...> d,
                    typename detail::PassArgument<ArgTypes>::type... vArgs)
      : diagID(d.diagID) {

    auto diagArgs = {std::move(vArgs)...};
    args.append(diagArgs + 1, diagArgs + 1 + sizeof...(vArgs));
  }

public:
  DiagnosticContext(DiagID diagID, llvm::ArrayRef<DiagnosticArgument> arguments)
      : diagID(diagID), args(arguments.begin(), arguments.end()) {}

public:
  DiagID GetDiagID() { return diagID; }
  llvm::ArrayRef<DiagnosticArgument> GetArgs() const { return args; }
  llvm::ArrayRef<CharSrcRange> GetRanges() const { return ranges; }
  llvm::ArrayRef<FixHint> GetFixHints() const { return hints; }

public:
  void AddRange(CharSrcRange range) { ranges.push_back(range); }
  // Avoid copying the fix-it text more than necessary.
  void AddFixHint(FixHint &&fix) { hints.push_back(std::move(fix)); }
};
class Diagnostic {
  SrcLoc loc;
  DiagnosticContext diagContext;

  friend class DiagnosticEngine;
  friend class LiveDiagnostic;

public:
  explicit Diagnostic(DiagnosticContext diagContext)
      : diagContext(diagContext) {}

public:
  void SetLoc(SrcLoc sl) { loc = sl; }
  SrcLoc GetLoc() { return loc; }

  DiagnosticContext &GetDiagContext() { return diagContext; }

  /// The result is appended onto the \p OutStr array.
  virtual void Format(llvm::SmallVectorImpl<char> &outStr,
                      const DiagnosticFormatOptions &fmtOptions) const;

  /// Format the given format-string into the output buffer using the
  /// arguments stored in this diagnostic.
  virtual void Format(const char *diagStr, const char *diagEnd,
                      llvm::SmallVectorImpl<char> &outStr,
                      const DiagnosticFormatOptions &fmtOptions) const;
};

} // namespace stone
#endif
