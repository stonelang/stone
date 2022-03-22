#ifndef STONE_CORE_DIAGNOSTIC_H
#define STONE_CORE_DIAGNOSTIC_H

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

#include "stone/Core/DiagnosticArgument.h"
#include "stone/Core/DiagnosticOptions.h"
#include "stone/Core/SrcMgr.h"
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
template <typename T> struct PassArgument { typedef T type; };
} // namespace detail

class CodeFix final {
  CharSrcRange range;
  /// The new code to insert at the insertion location.
  llvm::StringRef code;

public:
  CodeFix(CharSrcRange range, llvm::StringRef code);

  CharSrcRange GetRange() const { return range; }
  llvm::StringRef GetCode() const { return code; }
};

class CodeFixer final {
  friend class InFlightDiagnostic;
  InFlightDiagnostic &inFlightDiag;

public:
  CodeFixer(InFlightDiagnostic &inFlightDiag) : inFlightDiag(inFlightDiag) {}

public:
  static llvm::StringRef GetFixIDString(const FixID fixID);
  /// Prevent the diagnostic from behaving more severely than \p limit. For
  /// instance, if \c DiagnosticBehavior::Warning is passed, an error will be
  /// emitted as a warning, but a note will still be emitted as a note.
  InFlightDiagnostic &CapDiagLevel(diag::Level level);

  /// Add a token-based range to the currently-active diagnostic.
  InFlightDiagnostic &Highlight(SrcRange R);

  /// Add a character-based range to the currently-active diagnostic.
  InFlightDiagnostic &HighlightChars(SrcLoc Start, SrcLoc End);

  template <typename... ArgTypes>
  InFlightDiagnostic &
  Replace(SrcRange R, Fix<ArgTypes...> fixIt,
          typename detail::PassArgument<ArgTypes>::type... VArgs) {
    diag::Argument DiagArgs[] = {std::move(VArgs)...};
    return Replace(R, GetFixIDString(fixIt.ID), DiagArgs);
  }

  /// Add a character-based replacement fix-it to the currently-active
  /// diagnostic.
  template <typename... ArgTypes>
  InFlightDiagnostic &
  ReplaceChars(SrcLoc Start, SrcLoc End, Fix<ArgTypes...> fixIt,
               typename detail::PassArgument<ArgTypes>::type... VArgs) {
    diag::Argument DiagArgs[] = {std::move(VArgs)...};
    return ReplaceChars(Start, End, GetFixIDString(fixIt.ID), DiagArgs);
  }

  /// Add an insertion fix-it to the currently-active diagnostic.
  template <typename... ArgTypes>
  InFlightDiagnostic &
  Insert(SrcLoc L, Fix<ArgTypes...> fixIt,
         typename detail::PassArgument<ArgTypes>::type... VArgs) {
    diag::Argument DiagArgs[] = {std::move(VArgs)...};
    return ReplaceChars(L, L, GetFixIDString(fixIt.ID), DiagArgs);
  }

  /// Add an insertion fix-it to the currently-active diagnostic.  The
  /// text is inserted immediately *after* the token specified.
  template <typename... ArgTypes>
  InFlightDiagnostic &
  InsertAfter(SrcLoc L, Fix<ArgTypes...> fixIt,
              typename detail::PassArgument<ArgTypes>::type... VArgs) {
    diag::Argument DiagArgs[] = {std::move(VArgs)...};
    return InsertAfter(L, GetFixIDString(fixIt.ID), DiagArgs);
  }

  /// Add a token-based replacement fix-it to the currently-active
  /// diagnostic.
  InFlightDiagnostic &Replace(SrcRange R, StringRef Str);

  /// Add a character-based replacement fix-it to the currently-active
  /// diagnostic.
  InFlightDiagnostic &ReplaceChars(SrcLoc Start, SrcLoc End, StringRef Str) {
    return ReplaceChars(Start, End, "%0", diag::LLVMStrArgument{Str});
  }

  /// Add an insertion fix-it to the currently-active diagnostic.
  InFlightDiagnostic &Insert(SrcLoc L, StringRef Str) {
    return ReplaceChars(L, L, "%0", diag::LLVMStrArgument{Str});
  }

  /// Add an insertion fix-it to the currently-active diagnostic. The
  /// text is inserted immediately *after* the token specified.
  InFlightDiagnostic &InsertAfter(SrcLoc L, StringRef Str) {
    return InsertAfter(L, "%0", diag::LLVMStrArgument{Str});
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
                              ArrayRef<diag::Argument> Args);

  InFlightDiagnostic &ReplaceChars(SrcLoc Start, SrcLoc End,
                                   StringRef FormatString,
                                   ArrayRef<diag::Argument> Args);

  InFlightDiagnostic &Insert(SrcLoc L, StringRef FormatString,
                             ArrayRef<diag::Argument> Args) {
    return ReplaceChars(L, L, FormatString, Args);
  }

  InFlightDiagnostic &InsertAfter(SrcLoc L, StringRef FormatString,
                                  ArrayRef<diag::Argument> Args);
};

struct DiagnosticFormatOptions final {};

// TODO: Free Context
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

  void Clear() {
    args.clear();
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
  DiagnosticContext &GetContext() const { return context; }

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
                     llvm::StringRef formatMessage, llvm::StringRef category)
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
