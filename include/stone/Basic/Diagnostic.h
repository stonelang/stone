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
  // This ID will be used to look up the string vis GetDiagString(DiagID ...)
  DiagID diagID;
  SrcLoc loc;
  diag::Level level = diag::Level::None;
  llvm::SmallVector<diag::Argument, 3> args;
  llvm::SmallVector<CharSrcRange, 2> ranges;
  llvm::SmallVector<CodeFix, 2> fixes;

public:
  template <typename... ArgTypes>
  DiagnosticContext(Diag<ArgTypes...> d,
                    typename detail::PassArgument<ArgTypes>::type... vArgs)
      : diagID(d.diagID) {

    diag::Argument diagArgs[] = {std::forward<ArgTypes>(vArgs)...};

    args.append(diagArgs + 1, diagArgs + 1 + sizeof...(vArgs));
  }

public:
  DiagnosticContext(DiagID diagID, llvm::ArrayRef<diag::Argument> arguments)
      : diagID(diagID), args(arguments.begin(), arguments.end()) {}

public:
  DiagID GetDiagID() { return diagID; }
  llvm::ArrayRef<diag::Argument> GetArgs() const { return args; }
  llvm::ArrayRef<CharSrcRange> GetRanges() const { return ranges; }
  llvm::ArrayRef<CodeFix> GetFixes() const { return fixes; }

public:
  void AddRange(CharSrcRange range) { ranges.push_back(range); }
  // Avoid copying the fix-it text more than necessary.
  void AddFix(CodeFix &&fix) { fixes.push_back(std::move(fix)); }

  void AddArgument(diag::Argument &&arg) { args.push_back(std::move(arg)); }

  void SetLoc(SrcLoc sl) { loc = sl; }
  SrcLoc GetLoc() { return loc; }

  void SetLevel(diag::Level l) { level = l; }
  diag::Level GetLevel() { return level; }

  void Flush() {
    ranges.clear();
    fixes.clear();
  }
};
class Diagnostic {
protected:
  mutable DiagnosticContext context;

public:
  explicit Diagnostic(DiagnosticContext context) : context(context) {}

public:
  DiagnosticContext &GetContext() { return context; }

  // TODO: UB
  void AddChild(Diagnostic &&diagnostic);

public:
  template <typename... otherArgTypes>
  bool IsEqual(Diag<otherArgTypes...> other) const {
    return context.GetDiagID() == other.GetContext().GetDiagID();
  }

public:
  /// The result is appended onto the \p OutStr array.
  virtual void Format(llvm::SmallVectorImpl<char> &outStr,
                      const DiagnosticFormatOptions &fmtOptions) const;

  /// Format the given format-string into the output buffer using the
  /// arguments stored in this diagnostic.
  virtual void Format(const char *diagStr, const char *diagEnd,
                      llvm::SmallVectorImpl<char> &outStr,
                      const DiagnosticFormatOptions &fmtOptions) const;
};

class EmissionDiagnostic final {

  llvm::StringRef category;
  llvm::StringRef formatMessage;
  const Diagnostic &diagnostic;

public:
  EmissionDiagnostic(const Diagnostic &diagnostic,
                     llvm::StringRef formatMessage, llvm::StringRef categor)
      : diagnostic(diagnostic), formatMessage(formatMessage),
        category(category) {}

public:
  llvm::StringRef GetCategory() { return category; }
  llvm::StringRef GetFormatMessage() { return formatMessage; }
  const Diagnostic &GetDiagnostic() const { return diagnostic; }

public:
  // TODO: Think about
  void Format();
};

} // namespace stone
#endif
