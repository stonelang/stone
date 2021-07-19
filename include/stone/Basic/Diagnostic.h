#ifndef STONE_BASIC_DIAGNOSTIC_H
#define STONE_BASIC_DIAGNOSTIC_H

#include "stone/Basic/DiagnosticArgument.h"
#include "stone/Basic/DiagnosticOptions.h"
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
class Diagnostic;
class InFlightDiagnostic;
class DiagnoticEngine;

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

class CodeFix final {
  /// Code that should be replaced to correct the error. Empty for an
  /// insertion hint.
  CharSrcRange removeRange;

  /// Code in the specific range that should be inserted in the insertion
  /// location.
  CharSrcRange insertFromRange;

  /// The actual code to insert at the insertion location, as a
  /// string.
  llvm::StringRef code;

  bool beforePreviousInsertions = false;

public:
  CodeFix(CharSrcRange removeRange, CharSrcRange insertFromRange,
          llvm::StringRef code, bool beforePreviousInsertions = false);

public:
  CharSrcRange GetRemoveRange() const { return removeRange; }
  CharSrcRange GetInsertFromRange() const { return removeRange; }
  llvm::StringRef GetCode() const { return code; }
  bool IsBeforePreviousInsertions() { return beforePreviousInsertions; }

  bool IsNull() const { return !removeRange.isValid(); }
};
class CodeFixer final {
  friend class InFlightDiagnostic;
  InFlightDiagnostic &inFlightDiag;

public:
  CodeFixer(InFlightDiagnostic &inFlightDiag) : inFlightDiag(inFlightDiag) {}

public:
  /// Create a code fix that inserts the given code string at a specific
  /// location.
  InFlightDiagnostic InsertFromLoc(SrcLoc insertionLoc, StringRef code,
                                   bool beforePreviousInsertions = false);

  /// Create a code fixthat inserts the given
  /// code from \p FromRange at a specific location.
  InFlightDiagnostic InsertFromRange(SrcLoc insertionLoc,
                                     CharSrcRange fromRange,
                                     bool beforePreviousInsertions = false);

  /// Create a code fix that removes the given source range.
  InFlightDiagnostic RemoveRange(CharSrcRange removeRange);
  InFlightDiagnostic RemoveRange(SrcRange removeRange);

  /// Create a code fix that replaces the given source range with the given code
  /// string.
  InFlightDiagnostic Replace(CharSrcRange removeRange, llvm::StringRef code);
  InFlightDiagnostic Replace(SrcRange removeRange, llvm::StringRef code);

  /// Add a token-based range to the currently-active diagnostic.
  InFlightDiagnostic Highlight(SrcRange range);

  /// Add a character-based range to the currently-active diagnostic.
  InFlightDiagnostic HighlightChars(SrcLoc sartLoc, SrcLoc endLoc);
};
struct DiagnosticFormatOptions final {};

/// Pass the D
class DiagnosticContext {
  DiagID diagID;
  llvm::SmallVector<DiagnosticArgument, 3> args;
  llvm::SmallVector<CharSrcRange, 2> ranges;
  llvm::SmallVector<CodeFix, 2> fixes;

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
  llvm::ArrayRef<CodeFix> GetFixes() const { return fixes; }

public:
  void AddRange(CharSrcRange range) { ranges.push_back(range); }
  // Avoid copying the fix-it text more than necessary.
  void AddFix(CodeFix &&fix) { fixes.push_back(std::move(fix)); }

  void Flush() {
    ranges.clear();
    fixes.clear();
  }
};
class Diagnostic {
protected:
  SrcLoc loc;
  diag::Level level = diag::Level::None;
  mutable DiagnosticContext context;

public:
  explicit Diagnostic(DiagnosticContext context) : context(context) {}

public:
  void SetLoc(SrcLoc sl) { loc = sl; }
  SrcLoc GetLoc() { return loc; }

  /// TODO: move to context
  void SetLevel(diag::Level l) { level = l; }
  diag::Level GetLevel() { return level; }

  DiagnosticContext &GetContext() { return context; }

public:
  template <typename... otherArgTypes>
  bool IsEqual(Diag<otherArgTypes...> other) const {
    return context.GetDiagID() == other.GetContext().GetDiagID();
  }

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
