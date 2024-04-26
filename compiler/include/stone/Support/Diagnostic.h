#ifndef STONE_DIAG_DIAGNOSTIC_H
#define STONE_DIAG_DIAGNOSTIC_H

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

#include "stone/Basic/SrcMgr.h"
#include "stone/Support/DiagnosticArgument.h"
#include "stone/Support/DiagnosticOptions.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/IntrusiveRefCntPtr.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/Error.h"

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
template <typename T> struct PassArgument {
  typedef T type;
};
} // namespace detail

class DiagnosticFix final {
  CharSrcRange range;
  /// The new code to insert at the insertion location.
  llvm::StringRef code;

public:
  DiagnosticFix(CharSrcRange range, llvm::StringRef code);

  CharSrcRange GetRange() const { return range; }
  llvm::StringRef GetCode() const { return code; }
};

class DiagnosticFixer final {
  friend class InFlightDiagnostic;
  InFlightDiagnostic &inFlightDiag;

public:
  DiagnosticFixer(InFlightDiagnostic &inFlightDiag)
      : inFlightDiag(inFlightDiag) {}

public:
  static llvm::StringRef GetFixIDString(const FixID fixID);
  /// Prevent the diagnostic from behaving more severely than \p limit. For
  /// instance, if \c DiagnosticBehavior::Warning is passed, an error will be
  /// emitted as a warning, but a note will still be emitted as a note.
  InFlightDiagnostic &CapDiagLevel(DiagnosticLevel level);

  /// Add a token-based range to the currently-active diagnostic.
  InFlightDiagnostic &Highlight(SrcRange R);

  /// Add a character-based range to the currently-active diagnostic.
  InFlightDiagnostic &HighlightChars(SrcLoc Start, SrcLoc End);

  template <typename... ArgTypes>
  InFlightDiagnostic &
  Replace(SrcRange R, Fix<ArgTypes...> fixIt,
          typename detail::PassArgument<ArgTypes>::type... VArgs) {
    DiagnosticArgument DiagArgs[] = {std::move(VArgs)...};
    return Replace(R, GetFixIDString(fixIt.ID), DiagArgs);
  }

  /// Add a character-based replacement fix-it to the currently-active
  /// diagnostic.
  template <typename... ArgTypes>
  InFlightDiagnostic &
  ReplaceChars(SrcLoc Start, SrcLoc End, Fix<ArgTypes...> fixIt,
               typename detail::PassArgument<ArgTypes>::type... VArgs) {
    DiagnosticArgument DiagArgs[] = {std::move(VArgs)...};
    return ReplaceChars(Start, End, GetFixIDString(fixIt.ID), DiagArgs);
  }

  /// Add an insertion fix-it to the currently-active diagnostic.
  template <typename... ArgTypes>
  InFlightDiagnostic &
  Insert(SrcLoc L, Fix<ArgTypes...> fixIt,
         typename detail::PassArgument<ArgTypes>::type... VArgs) {
    DiagnosticArgument DiagArgs[] = {std::move(VArgs)...};
    return ReplaceChars(L, L, GetFixIDString(fixIt.ID), DiagArgs);
  }

  /// Add an insertion fix-it to the currently-active diagnostic.  The
  /// text is inserted immediately *after* the token specified.
  template <typename... ArgTypes>
  InFlightDiagnostic &
  InsertAfter(SrcLoc L, Fix<ArgTypes...> fixIt,
              typename detail::PassArgument<ArgTypes>::type... VArgs) {
    DiagnosticArgument DiagArgs[] = {std::move(VArgs)...};
    return InsertAfter(L, GetFixIDString(fixIt.ID), DiagArgs);
  }

  /// Add a token-based replacement fix-it to the currently-active
  /// diagnostic.
  InFlightDiagnostic &Replace(SrcRange R, StringRef Str);

  /// Add a character-based replacement fix-it to the currently-active
  /// diagnostic.
  InFlightDiagnostic &ReplaceChars(SrcLoc Start, SrcLoc End, StringRef Str) {
    return ReplaceChars(Start, End, "%0", DiagnosticArgument(Str));
  }

  /// Add an insertion fix-it to the currently-active diagnostic.
  InFlightDiagnostic &Insert(SrcLoc L, StringRef Str) {
    return ReplaceChars(L, L, "%0", DiagnosticArgument(Str));
  }

  /// Add an insertion fix-it to the currently-active diagnostic. The
  /// text is inserted immediately *after* the token specified.
  InFlightDiagnostic &InsertAfter(SrcLoc L, StringRef Str) {
    return InsertAfter(L, "%0", DiagnosticArgument(Str));
  }

  /// Add a token-based removal fix-it to the currently-active
  /// diagnostic.
  InFlightDiagnostic &Remove(SrcRange R);

  /// Add a character-based removal fix-it to the currently-active
  /// diagnostic.
  InFlightDiagnostic &RemoveChars(SrcLoc Start, SrcLoc End) {
    return ReplaceChars(Start, End, {});
  }

  /// Add two replacement fix-it exchanging source ranges to the
  /// currently-active diagnostic.
  InFlightDiagnostic &Exchange(SrcRange R1, SrcRange R2);

private:
  InFlightDiagnostic &Replace(SrcRange R, StringRef FormatString,
                              ArrayRef<DiagnosticArgument> Args);

  InFlightDiagnostic &ReplaceChars(SrcLoc Start, SrcLoc End,
                                   StringRef FormatString,
                                   ArrayRef<DiagnosticArgument> Args);

  InFlightDiagnostic &Insert(SrcLoc L, StringRef FormatString,
                             ArrayRef<DiagnosticArgument> Args) {
    return ReplaceChars(L, L, FormatString, Args);
  }

  InFlightDiagnostic &InsertAfter(SrcLoc L, StringRef FormatString,
                                  ArrayRef<DiagnosticArgument> Args);
};

class Diagnostic {

  friend class DiagnosticEngine;
  friend class InFlightDiagnostic;
  // This ID will be used to look up the string vis GetDiagString(DiagID ...)
  DiagID diagID;

  SrcLoc loc;
  DiagnosticLevel levelLimit = DiagnosticLevel::None;
  llvm::SmallVector<DiagnosticArgument, 3> args;
  llvm::SmallVector<CharSrcRange, 2> ranges;
  llvm::SmallVector<DiagnosticFix, 2> fixes;
  // llvm::SmallVector<Diagnostic *> deps;

public:
  template <typename... ArgTypes>
  Diagnostic(Diag<ArgTypes...> d,
             typename detail::PassArgument<ArgTypes>::type... vArgs)
      : diagID(d.diagID) {
    DiagnosticArgument diagArgs[] = {std::forward<ArgTypes>(vArgs)...};

    args.append(diagArgs + 1, diagArgs + 1 + sizeof...(vArgs));
  }

public:
  Diagnostic(DiagID diagID, llvm::ArrayRef<DiagnosticArgument> args)
      : diagID(diagID), args(args.begin(), args.end()) {}

public:
  DiagID GetID() const { return diagID; }
  llvm::ArrayRef<DiagnosticArgument> GetArgs() const { return args; }
  llvm::ArrayRef<CharSrcRange> GetRanges() const { return ranges; }
  llvm::ArrayRef<DiagnosticFix> GetFixes() const { return fixes; }
  // llvm::ArrayRef<Diagnostic *> GetDeps() const { return deps; }

public:
  void AddRange(CharSrcRange range) { ranges.push_back(range); }
  // Avoid copying the fix-it text more than necessary.
  void AddFix(DiagnosticFix &&fix) { fixes.push_back(std::move(fix)); }

  // void AddDep(Diagnostic *dep) { deps.push_back(dep); }

  void AddArgument(DiagnosticArgument &&arg) { args.push_back(std::move(arg)); }

  void SetLoc(SrcLoc sl) { loc = sl; }
  SrcLoc GetLoc() { return loc; }

  void SetLevelLimit(DiagnosticLevel limit) { levelLimit = limit; }
  DiagnosticLevel GetLevelLimit() const { return levelLimit; }

public:
  template <typename... otherArgTypes>
  bool IsEqual(Diag<otherArgTypes...> other) const {
    return GetID() == other.GetID();
  }

  void Clear() {
    args.clear();
    ranges.clear();
    fixes.clear();
  }
};

class DiagnosticMessage final {
  DiagnosticLevel level;
  llvm::StringRef category;
  llvm::StringRef formatMessage;
  const Diagnostic &diagnostic;
  SrcMgr &sm;

public:
  DiagnosticMessage(DiagnosticLevel level, const Diagnostic &diagnostic,
                    SrcMgr &sm, llvm::StringRef formatMessage,
                    llvm::StringRef category)
      : level(level), diagnostic(diagnostic), sm(sm),
        formatMessage(formatMessage), category(category) {}

public:
  llvm::StringRef GetCategory() { return category; }
  llvm::StringRef GetFormatMessage() { return formatMessage; }
  const Diagnostic &GetDiagnostic() const { return diagnostic; }
  SrcMgr &GetSrcMgr() { return sm; }
  DiagnosticLevel GetLevel() { return level; }

  // public:
  //   // TODO: Think about
  //   void Format();
};

} // namespace stone
#endif
