#ifndef STONE_SYNTAX_DIAGNOSTICS_H
#define STONE_SYNTAX_DIAGNOSTICS_H

#include "stone/Syntax/Identifier.h"
#include "stone/Syntax/TypeLoc.h"

#include "llvm/ADT/BitVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/StringSet.h"
#include "llvm/Support/Allocator.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/SaveAndRestore.h"
#include "llvm/Support/VersionTuple.h"

namespace clang {
class NamedDecl;
}

namespace stone {
class Decl;
class ConstructorDecl;
class FuncDecl;
class AliasDecl;
class ValueDecl;
class DeclAttribute;
class DiagnosticEngine;
class Identifier;
class GeneratedSourceInfo;
class SrcMgr;
class SourceFile;

namespace diags {
class DiagnosticArgument;

enum class DescriptivePatternKind : uint8_t;
enum class SelfAccessKind : uint8_t;
enum class ReferenceOwnership : uint8_t;
enum class StaticSpellingKind : uint8_t;
enum class DescriptiveDeclKind : uint8_t;
enum class DeclAttrKind : unsigned;
enum class StmtKind;

/// Describes the current behavior to take with a diagnostic.
/// Ordered from most severe to least.
enum class DiagnosticBehavior : uint8_t {
  Unspecified = 0,
  Fatal,
  Error,
  Warning,
  Remark,
  Note,
  Ignore,
};

// Enumeration describing all of possible diagnostics.
///
/// Each of the diagnostics described in Diagnostics.def has an entry in
/// this enumeration type that uniquely identifies it.
enum class DiagID : uint32_t;

/// Describes a diagnostic along with its argument types.
///
/// The diagnostics header introduces instances of this type for each
/// diagnostic, which provide both the set of argument types (used to
/// check/convert the arguments at each call site) and the diagnostic ID
/// (for other information about the diagnostic).
template <typename... ArgTypes> struct Diag {
  /// The diagnostic ID corresponding to this diagnostic.
  DiagID ID;
};

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

template <class... ArgTypes>
using DiagArgTuple =
    std::tuple<typename detail::PassArgument<ArgTypes>::type...>;

/// A family of wrapper types for compiler data types that forces its
/// underlying data to be formatted with full qualification.
///
/// So far, this is only useful for \c Type, hence the SFINAE'ing.
template <typename T, typename = void> struct FullyQualified {};

template <typename T>
struct FullyQualified<
    T,
    typename std::enable_if<std::is_convertible<T, stone::Type>::value>::type> {
  stone::Type t;

public:
  FullyQualified(T t) : t(t){};
  stone::Type getType() const { return t; }
};

enum class DiagnosticArgumentKind {
  String,
  Integer,
  Unsigned,
  Identifier,
  Decl,
  Type,
  VersionTuple,
  Diagnostic,
  ClangDecl
};
enum class RequirementKind : uint8_t;

/// Describes the kind of diagnostic.
///
enum class DiagnosticKind : uint8_t { Error, Warning, Remark, Note };

/// Information about a diagnostic passed to DiagnosticConsumers.
struct DiagnosticInfo final {
  DiagID ID = DiagID(0);
  SrcLoc Loc;
  DiagnosticKind Kind;
  StringRef FormatString;
  ArrayRef<DiagnosticArgument> FormatArgs;
  StringRef Category;

  /// Only used when directing diagnostics to different outputs.
  /// In batch mode a diagnostic may be
  /// located in a non-primary file, but there will be no .dia file for a
  /// non-primary. If valid, this argument contains a location within a buffer
  /// that corresponds to a primary input. The .dia file for that primary can be
  /// used for the diagnostic, as if it had occurred at this location.
  SrcLoc BufferIndirectlyCausingDiagnostic;

  /// DiagnosticInfo of notes which are children of this diagnostic, if any
  ArrayRef<DiagnosticInfo *> ChildDiagnosticInfo;

  /// Paths to "educational note" diagnostic documentation in the toolchain.
  ArrayRef<std::string> EducationalNotePaths;

  /// Represents a fix-it, a replacement of one range of text with another.
  class FixIt {
    CharSrcRange Range;
    std::string Text;

  public:
    FixIt(CharSrcRange R, StringRef Str, ArrayRef<DiagnosticArgument> Args);

    CharSrcRange &getRange() { return Range; }
    const CharSrcRange &getRange() const { return Range; }

    StringRef getText() const { return Text; }
  };

  /// Extra source ranges that are attached to the diagnostic.
  ArrayRef<CharSrcRange> Ranges;

  /// Extra source ranges that are attached to the diagnostic.
  ArrayRef<FixIt> FixIts;

  /// This is a note which has a parent error or warning
  bool IsChildNote = false;

  DiagnosticInfo() {}

  DiagnosticInfo(DiagID ID, SrcLoc Loc, DiagnosticKind Kind,
                 StringRef FormatString,
                 ArrayRef<DiagnosticArgument> FormatArgs, StringRef Category,
                 SrcLoc BufferIndirectlyCausingDiagnostic,
                 ArrayRef<DiagnosticInfo *> ChildDiagnosticInfo,
                 ArrayRef<CharSrcRange> Ranges, ArrayRef<FixIt> FixIts,
                 bool IsChildNote)
      : ID(ID), Loc(Loc), Kind(Kind), FormatString(FormatString),
        FormatArgs(FormatArgs), Category(Category),
        BufferIndirectlyCausingDiagnostic(BufferIndirectlyCausingDiagnostic),
        ChildDiagnosticInfo(ChildDiagnosticInfo), Ranges(Ranges),
        FixIts(FixIts), IsChildNote(IsChildNote) {}
};

/// Variant type that holds a single diagnostic argument of a known
/// type.
///
/// All diagnostic arguments are converted to an instance of this class.
class DiagnosticArgument final {
  DiagnosticArgumentKind Kind;
  union {
    int IntegerVal;
    unsigned UnsignedVal;
    StringRef StringVal;
    Identifier IdentifierVal;
    const Decl *TheDecl;
    Type TypeVal;
    DiagnosticInfo *DiagnosticVal;
  };

public:
  DiagnosticArgument(StringRef S)
      : Kind(DiagnosticArgumentKind::String), StringVal(S) {}

  DiagnosticArgument(int I)
      : Kind(DiagnosticArgumentKind::Integer), IntegerVal(I) {}

  DiagnosticArgument(unsigned I)
      : Kind(DiagnosticArgumentKind::Unsigned), UnsignedVal(I) {}

  DiagnosticArgument(Identifier I)
      : Kind(DiagnosticArgumentKind::Identifier), IdentifierVal(Identifier(I)) {
  }

  DiagnosticArgument(const Decl *VD)
      : Kind(DiagnosticArgumentKind::Decl), TheDecl(VD) {}

  DiagnosticArgument(Type T) : Kind(DiagnosticArgumentKind::Type), TypeVal(T) {}

  /// Initializes a diagnostic argument using the underlying type of the
  /// given enum.
  template <
      typename EnumType,
      typename std::enable_if<std::is_enum<EnumType>::value>::type * = nullptr>
  DiagnosticArgument(EnumType value)
      : DiagnosticArgument(
            static_cast<typename std::underlying_type<EnumType>::type>(value)) {
  }

  DiagnosticArgumentKind getKind() const { return Kind; }

  StringRef getAsString() const {
    assert(Kind == DiagnosticArgumentKind::String);
    return StringVal;
  }

  int getAsInteger() const {
    assert(Kind == DiagnosticArgumentKind::Integer);
    return IntegerVal;
  }

  unsigned getAsUnsigned() const {
    assert(Kind == DiagnosticArgumentKind::Unsigned);
    return UnsignedVal;
  }

  Identifier getAsIdentifier() const {
    assert(Kind == DiagnosticArgumentKind::Identifier);
    return IdentifierVal;
  }

  const Decl *getAsDecl() const {
    assert(Kind == DiagnosticArgumentKind::Decl);
    return TheDecl;
  }

  Type getAsType() const {
    assert(Kind == DiagnosticArgumentKind::Type);
    return TypeVal;
  }
};

struct DiagnosticFormatOptions {
  const std::string OpeningQuotationMark;
  const std::string ClosingQuotationMark;
  const std::string AKAFormatString;
  const std::string OpaqueResultFormatString;

  DiagnosticFormatOptions(std::string OpeningQuotationMark,
                          std::string ClosingQuotationMark,
                          std::string AKAFormatString,
                          std::string OpaqueResultFormatString)
      : OpeningQuotationMark(OpeningQuotationMark),
        ClosingQuotationMark(ClosingQuotationMark),
        AKAFormatString(AKAFormatString),
        OpaqueResultFormatString(OpaqueResultFormatString) {}

  DiagnosticFormatOptions()
      : OpeningQuotationMark("'"), ClosingQuotationMark("'"),
        AKAFormatString("'%s' (aka '%s')"),
        OpaqueResultFormatString("'%s' (%s of '%s')") {}

  /// When formatting fix-it arguments, don't include quotes or other
  /// additions which would result in invalid code.
  static DiagnosticFormatOptions formatForFixIts() {
    return DiagnosticFormatOptions("", "", "%s", "%s");
  }
};

enum class FixItID : uint32_t;

/// Represents a fix-it defined  with a format string and optional
/// DiagnosticArguments. The template parameters allow the
/// fixIt... methods on InFlightDiagnostic to infer their own
/// template params.
template <typename... ArgTypes> struct StructuredFixIt {
  FixItID ID;
};

/// Diagnostic - This is a specific instance of a diagnostic along with all of
/// the DiagnosticArguments that it requires.
class Diagnostic {
public:
  typedef DiagnosticInfo::FixIt FixIt;

private:
  DiagID ID;
  SmallVector<DiagnosticArgument, 3> Args;
  SmallVector<CharSrcRange, 2> Ranges;
  SmallVector<FixIt, 2> FixIts;
  std::vector<Diagnostic> ChildNotes;
  SrcLoc Loc;
  bool IsChildNote = false;
  const stone::Decl *Decl = nullptr;
  DiagnosticBehavior BehaviorLimit = DiagnosticBehavior::Unspecified;

  friend DiagnosticEngine;
  friend class InFlightDiagnostic;

  Diagnostic(DiagID ID) : ID(ID) {}

public:
  // All constructors are intentionally implicit.
  template <typename... ArgTypes>
  Diagnostic(Diag<ArgTypes...> ID,
             typename detail::PassArgument<ArgTypes>::type... VArgs)
      : Diagnostic(ID.ID) {
    Args.reserve(sizeof...(ArgTypes));
    gatherArgs(VArgs...);
  }

  /*implicit*/ Diagnostic(DiagID ID, ArrayRef<DiagnosticArgument> Args)
      : ID(ID), Args(Args.begin(), Args.end()) {}

  template <class... ArgTypes>
  static Diagnostic fromTuple(Diag<ArgTypes...> id,
                              const DiagArgTuple<ArgTypes...> &tuple) {
    Diagnostic result(id.ID);
    result.gatherArgsFromTuple<DiagArgTuple<ArgTypes...>, 0, ArgTypes...>(
        tuple);
    return result;
  }

  // Accessors.
  DiagID getID() const { return ID; }
  ArrayRef<DiagnosticArgument> getArgs() const { return Args; }
  ArrayRef<CharSrcRange> getRanges() const { return Ranges; }
  ArrayRef<FixIt> getFixIts() const { return FixIts; }
  ArrayRef<Diagnostic> getChildNotes() const { return ChildNotes; }
  bool isChildNote() const { return IsChildNote; }
  SrcLoc getLoc() const { return Loc; }
  const class Decl *getDecl() const { return Decl; }
  DiagnosticBehavior getBehaviorLimit() const { return BehaviorLimit; }

  void setLoc(SrcLoc loc) { Loc = loc; }
  void setIsChildNote(bool isChildNote) { IsChildNote = isChildNote; }
  void setDecl(const class Decl *decl) { Decl = decl; }
  void setBehaviorLimit(DiagnosticBehavior limit) { BehaviorLimit = limit; }

  /// Returns true if this object represents a particular diagnostic.
  ///
  /// \code
  /// someDiag.is(diag::invalid_diagnostic)
  /// \endcode
  template <typename... OtherArgTypes>
  bool is(Diag<OtherArgTypes...> Other) const {
    return ID == Other.ID;
  }

  void addRange(CharSrcRange R) { Ranges.push_back(R); }

  // Avoid copying the fix-it text more than necessary.
  void addFixIt(FixIt &&F) { FixIts.push_back(std::move(F)); }

  void addChildNote(Diagnostic &&D);
  void insertChildNote(unsigned beforeIndex, Diagnostic &&D);

private:
  // gatherArgs could just be `Args.emplace_back(args)...;` if C++
  // allowed pack expansions in statement context.

  // Base case.
  void gatherArgs() {}

  // Pull one off the pack.
  template <class ArgType, class... RemainingArgTypes>
  void gatherArgs(ArgType arg, RemainingArgTypes... remainingArgs) {
    Args.emplace_back(arg);
    gatherArgs(remainingArgs...);
  }

  // gatherArgsFromTuple could just be
  // `Args.emplace_back(std::get<packIndexOf<ArgTypes>>(tuple))...;`
  // in a better world.

  // Base case.
  template <class Tuple, size_t Index>
  void gatherArgsFromTuple(const Tuple &tuple) {}

  // Pull one off the pack.
  template <class Tuple, size_t Index, class ArgType,
            class... RemainingArgTypes>
  void gatherArgsFromTuple(const Tuple &tuple) {
    Args.emplace_back(std::move(std::get<Index>(tuple)));
    gatherArgsFromTuple<Tuple, Index + 1, RemainingArgTypes...>(
        std::move(tuple));
  }
};

/// A diagnostic that has no input arguments, so it is trivially-destructable.
using ZeroArgDiagnostic = Diag<>;

} // namespace diags

} // namespace stone

#endif