#ifndef STONE_DIAG_DIAGNOSTIC_CLIENT_H
#define STONE_DIAG_DIAGNOSTIC_CLIENT_H

#include "stone/AST/Identifier.h"
#include "stone/Basic/SrcMgr.h"
#include "stone/Diag/DiagnosticFormatParser.h"
#include "stone/Diag/DiagnosticID.h"
#include "stone/Support/DiagnosticOptions.h"

#include "llvm/ADT/BitVector.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/raw_ostream.h"

namespace stone {

class Diagnostic;
class DiagnosticEngine;

/// The level of the diagnostic, after it has been through mapping.
enum class DiagnosticLevel : uint8_t {
  None = 0,
  Ignore, /// Lowest
  Note,
  Remark,
  Warning,
  Error,
  Fatal, /// Highest
};

/// This is just the kind of diagnostic being process -- this is not the same as
/// the level.
enum class DiagnosticKind : uint8_t {
  Error = 0,
  Note,
  Remark,
  Warning,
};
enum class DiagnosticArgumentKind {
  None = 0,
  Integer,
  Unsigned,
  String,
  Identifier,
};
class DiagnosticArgument final {
  DiagnosticArgumentKind Kind;
  union {
    int IntegerVal;
    unsigned UnsignedVal;
    llvm::StringRef StringVal;
    Identifier IdentifierVal;
  };

public:
  DiagnosticArgument(llvm::StringRef S)
      : Kind(DiagnosticArgumentKind::String), StringVal(S) {}

  DiagnosticArgument(int I)
      : Kind(DiagnosticArgumentKind::Integer), IntegerVal(I) {}

  DiagnosticArgument(unsigned I)
      : Kind(DiagnosticArgumentKind::Unsigned), UnsignedVal(I) {}

  DiagnosticArgument(Identifier I)
      : Kind(DiagnosticArgumentKind::Identifier), IdentifierVal(I) {}

  /// Initializes a diagnostic argument using the underlying type of the
  /// given enum.
  template <
      typename EnumType,
      typename std::enable_if<std::is_enum<EnumType>::value>::type * = nullptr>
  DiagnosticArgument(EnumType value)
      : DiagnosticArgument(
            static_cast<typename std::underlying_type<EnumType>::type>(value)) {
  }

public:
  llvm::StringRef GetAsString() const {
    assert(Kind == DiagnosticArgumentKind::String);
    return StringVal;
  }

  int GetAsInteger() const {
    assert(Kind == DiagnosticArgumentKind::Integer);
    return IntegerVal;
  }

  unsigned GetAsUnsigned() const {
    assert(Kind == DiagnosticArgumentKind::Unsigned);
    return UnsignedVal;
  }

  Identifier GetAsIdentifier() const {
    assert(Kind == DiagnosticArgumentKind::Identifier);
    return IdentifierVal;
  }
};

class DiagnosticOutputStream final {
  friend class DiagnosticEngine;

  bool showColors;
  llvm::raw_ostream &OS;

  void ChangeColor(llvm::raw_ostream::Colors C) { OS.changeColor(C); }

public:
  DiagnosticOutputStream();
  DiagnosticOutputStream(llvm::raw_ostream &OS);
  ~DiagnosticOutputStream();

public:
  void ShowColors() { showColors = true; }
  void HideColors() { showColors = false; }
  void ResetColors();

  void UseNoteColor();
  void UseRemarkColor();
  void UseWarningColor();
  void UseErrorColor();
  void UseFatalColor();

  void UseFixItColor();
  void UseCaretColor();
  void UseTemplateColor();
  void UseSavedColor();
  void UseCommentColor();
  void UseLiteralColor();
  void UseKeywordColor();
  llvm::raw_ostream &GetOutputStream() { return OS; }
  void Flush() { OS.flush(); }
};

class FixIt final {
  CharSrcRange Range;
  std::string Text;

public:
  FixIt(CharSrcRange R, StringRef Str, llvm::ArrayRef<DiagnosticArgument> Args);

  CharSrcRange &GetRange() { return Range; }
  const CharSrcRange &GetRange() const { return Range; }

  StringRef GetText() const { return Text; }
};

// class DiagnosticFormatter {

//   llvm::SmallVectorImpl<char> &OutString;

// public:
//   DiagnosticFormatter(llvm::SmallVectorImpl<char> &OutString)
//       : OutString(OutString) {}

//   DiagnosticFormatter()
//       : DiagnosticFormatter(llvm::SmallString<64> OutString =
//       llvm::SmallString()) {}
// };

// For client consumption only
// class DiagnosticInfo final {

//   DiagnosticKind kind;
//   llvm::StringRef FormatString;
//   const DiagnosticEngine &DE;

//   Diagnostic(const Diagnostic &) = delete;
//   Diagnostic &operator=(const Diagnostic &) = delete;
//   Diagnostic &operator=(Diagnostic &&) = delete;

// public:
//   explicit Diagnostic(DiagnosticKind kind, llvm::StringRef FmtStr,
//                           const DiagnosticEngine &DE)
//       : kind(kind), FormatString(FmtStr), DE(DE) {}

// public:
//   DiagnosticKind GetKind() { return kind; }
//   llvm::StringRef GetFormatString() { return FormatString; }
//   const DiagnosticEngine &GetDiags() const { return DE; }

// public:
/// NOTE: use DiagnosticEngine::FormatDiagnosticText
///  Format this diagnostic into a string, substituting the
///  formal arguments into the %0 slots.
///
///  The result is appended onto the \p OutStr array.
// void FormatDiagnostic(llvm::SmallVectorImpl<char> &OutStr) const;

// /// Format the given format-string into the output buffer using the
// /// arguments stored in this diagnostic.
// void FormatDiagnostic(const char *DiagStr, const char *DiagEnd,
//                       llvm::SmallVectorImpl<char> &OutStr) const;

// Diagnostic ConstructDiagnostic();

//   void FormatDiagnostic(
//       llvm::raw_ostream &Out,
//       DiagnosticFormatOptions FormatOpts = DiagnosticFormatOptions()) const;
// };

class DiagnosticStringFormatter {
public:
};

/// Information about a diagnostic passed to DiagnosticConsumers.
struct DiagnosticImpl final {
  DiagID ID = DiagID(0);
  SrcLoc Loc;
  DiagnosticKind Kind;
  StringRef FormatString;
  ArrayRef<DiagnosticArgument> FormatArgs;

  /// DiagnosticInfo of notes which are children of this diagnostic, if any
  llvm::ArrayRef<Diagnostic *> ChildDiagnostics;

  /// Extra source ranges that are attached to the diagnostic.
  llvm::ArrayRef<CharSrcRange> Ranges;

  /// Extra source ranges that are attached to the diagnostic.
  llvm::ArrayRef<FixIt> FixIts;

  DiagnosticImpl(DiagID ID, SrcLoc Loc, DiagnosticKind Kind,
                 StringRef FormatString,
                 ArrayRef<DiagnosticArgument> FormatArgs,
                 ArrayRef<Diagnostic *> ChildDiagnostics,
                 ArrayRef<CharSrcRange> Ranges, ArrayRef<FixIt> FixIts)
      : ID(ID), Loc(Loc), Kind(Kind), FormatString(FormatString),
        FormatArgs(FormatArgs), ChildDiagnostics(ChildDiagnostics),
        Ranges(Ranges), FixIts(FixIts) {}

  // void FormatDiagnostic(
  //     DiagnosticFormatOptions FormatOpts = DiagnosticFormatOptions()) const;

  void FormatDiagnostic(
      llvm::raw_ostream &OS,
      DiagnosticFormatOptions FormatOpts = DiagnosticFormatOptions()) const;
};

/// Diagnostic
// class CustomDiagnostic {};

class DiagnosticClient {
  friend class DiagnosticEngine;

  unsigned TotalWarnings = 0; ///< Number of warnings reported
  unsigned TotalErrors = 0;   ///< Number of errors reported

  // DiagnosticTracker tracker;

public:
  DiagnosticClient();
  virtual ~DiagnosticClient();

public:
  virtual void Clear() { TotalWarnings = TotalErrors = 0; }

  /// Callback to inform the diagnostic client that processing of all
  /// source files has ended.
  virtual void FinishProcessing() {}

  /// Indicates whether the diagnostics handled by this
  /// DiagnosticConsumer should be included in the number of diagnostics
  /// reported by DiagnosticsEngine.
  ///
  /// The default implementation returns true.
  virtual bool UseInDiagnosticCounts() const;

  /// Handle this diagnostic, reporting it to the user or
  /// capturing it to a log as needed.
  ///
  /// The default implementation just keeps track of the total number of
  /// warnings and errors.
  virtual void HandleDiagnostic(SrcMgr &SM, const DiagnosticImpl &DI);

  // DiagnosticTracker &GetTracker() { return tracker; }
};

class NullDiagnosticClient final : public DiagnosticClient {
public:
  void HandleDiagnostic(SrcMgr &SM, const DiagnosticImpl &DI) override {
    // Just ignore it.
  }
};

/// Diagnostic consumer that forwards diagnostics along to an
/// existing, already-initialized diagnostic consumer.
///
class ForwardingDiagnosticClient final : public DiagnosticClient {

public:
  void HandleDiagnostic(SrcMgr &SM, const DiagnosticImpl &DI) override {
    // Just ignore it.
  }
};

} // namespace stone

#endif