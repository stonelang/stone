#include "stone/AST/Diagnostics.h"
#include "stone/AST/ASTContext.h"
#include "stone/AST/Decl.h"
// #include "stone/AST/DiagnosticSuppression.h"
#include "stone/AST/DiagnosticsBasic.h"
// #include "stone/AST/Expr.h"
#include "stone/AST/Module.h"
// #include "stone/AST/Pattern.h"
// #include "stone/AST/PrintOptions.h"
#include "stone/AST/Module.h"
#include "stone/AST/Stmt.h"
#include "stone/Basic/Range.h"

#include "stone/Basic/SrcMgr.h"
#include "stone/Support/LexerBase.h"

// #include "stone/Config.h"
// #include "stone/Localization/LocalizationFormat.h"

#include "clang/AST/ASTContext.h"
#include "clang/AST/Decl.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/Twine.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Format.h"
#include "llvm/Support/raw_ostream.h"

using namespace stone;

// static_assert(IsTriviallyDestructible<ZeroArgDiagnostic>::value,
//               "ZeroArgDiagnostic is meant to be trivially destructable");

namespace {

enum class LocalDiagnosticOptions {
  /// No options.
  none,

  /// The location of this diagnostic points to the beginning of the first
  /// token that the parser considers invalid.  If this token is located at the
  /// beginning of the line, then the location is adjusted to point to the end
  /// of the previous token.
  ///
  /// This behavior improves experience for "expected token X" diagnostics.
  PointsToFirstBadToken,

  /// After a fatal error subsequent diagnostics are suppressed.
  Fatal,

  /// An API or ABI breakage diagnostic emitted by the API digester.
  APIDigesterBreakage,

  /// A deprecation warning or error.
  Deprecation,

  /// A diagnostic warning about an unused element.
  NoUsage,
};

struct StoredDiagnosticInfo {
  DiagnosticKind kind : 2;
  bool pointsToFirstBadToken : 1;
  bool isFatal : 1;
  bool isAPIDigesterBreakage : 1;
  bool isDeprecation : 1;
  bool isNoUsage : 1;

  constexpr StoredDiagnosticInfo(DiagnosticKind k, bool firstBadToken,
                                 bool fatal, bool isAPIDigesterBreakage,
                                 bool deprecation, bool noUsage)
      : kind(k), pointsToFirstBadToken(firstBadToken), isFatal(fatal),
        isAPIDigesterBreakage(isAPIDigesterBreakage),
        isDeprecation(deprecation), isNoUsage(noUsage) {}
  constexpr StoredDiagnosticInfo(DiagnosticKind k, LocalDiagnosticOptions opts)
      : StoredDiagnosticInfo(
            k, opts == LocalDiagnosticOptions::PointsToFirstBadToken,
            opts == LocalDiagnosticOptions::Fatal,
            opts == LocalDiagnosticOptions::APIDigesterBreakage,
            opts == LocalDiagnosticOptions::Deprecation,
            opts == LocalDiagnosticOptions::NoUsage) {}
};

// Reproduce the DiagIDs, as we want both the size and access to the raw ids
// themselves.
enum LocalDiagID : uint32_t {
#define DIAG(KIND, ID, Options, Text, Signature) ID,
#include "stone/AST/Diagnostics.def"
  NumDiags
};

} // namespace

// TODO: categorization
static const constexpr StoredDiagnosticInfo storedDiagnosticInfos[] = {
#define ERROR(ID, Options, Text, Signature)                                    \
  StoredDiagnosticInfo(DiagnosticKind::Error, LocalDiagnosticOptions::Options),
#define WARNING(ID, Options, Text, Signature)                                  \
  StoredDiagnosticInfo(DiagnosticKind::Warning,                                \
                       LocalDiagnosticOptions::Options),
#define NOTE(ID, Options, Text, Signature)                                     \
  StoredDiagnosticInfo(DiagnosticKind::Note, LocalDiagnosticOptions::Options),
#define REMARK(ID, Options, Text, Signature)                                   \
  StoredDiagnosticInfo(DiagnosticKind::Remark, LocalDiagnosticOptions::Options),
#include "stone/AST/Diagnostics.def"
};
static_assert(sizeof(storedDiagnosticInfos) / sizeof(StoredDiagnosticInfo) ==
                  LocalDiagID::NumDiags,
              "array size mismatch");

static constexpr const char *const diagnosticStrings[] = {
#define DIAG(KIND, ID, Options, Text, Signature) Text,
#include "stone/AST/Diagnostics.def"
    "<not a diagnostic>",
};

static constexpr const char *const debugDiagnosticStrings[] = {
#define DIAG(KIND, ID, Options, Text, Signature) Text " [" #ID "]",
#include "stone/AST/Diagnostics.def"
    "<not a diagnostic>",
};

static constexpr const char *const diagnosticIDStrings[] = {
#define DIAG(KIND, ID, Options, Text, Signature) #ID,
#include "stone/AST/Diagnostics.def"
    "<not a diagnostic>",
};

static constexpr const char *const fixItStrings[] = {
#define DIAG(KIND, ID, Options, Text, Signature)
#define FIXIT(ID, Text, Signature) Text,
#include "stone/AST/Diagnostics.def"
    "<not a fix-it>",
};

DiagnosticState::DiagnosticState() {
  // Initialize our ignored diagnostics to default
  ignoredDiagnostics.resize(LocalDiagID::NumDiags);
}

/// Extract a character at \p Loc. If \p Loc is the end of the buffer,
/// return '\f'.
static char extractCharAfter(SrcMgr &SM, SrcLoc Loc) {
  auto chars = SM.extractText({Loc, 1});
  return chars.empty() ? '\f' : chars[0];
}

/// Extract a character immediately before \p Loc. If \p Loc is the
/// start of the buffer, return '\f'.
static char extractCharBefore(SrcMgr &SM, SrcLoc Loc) {
  // We have to be careful not to go off the front of the buffer.
  auto bufferID = SM.findBufferContainingLoc(Loc);
  auto bufferRange = SM.getRangeForBuffer(bufferID);
  if (bufferRange.getStart() == Loc)
    return '\f';
  auto chars = SM.extractText({Loc.getAdvancedLoc(-1), 1}, bufferID);
  assert(!chars.empty() && "Couldn't extractText with valid range");
  return chars[0];
}

CharSrcRange InFlightDiagnostic::toCharSrcRange(SrcMgr &SM, SrcRange SR) {
  return CharSrcRange(SM, SR.Start,
                      Engine->GetLexerBase()->GetLocForEndOfToken(SM, SR.End));
}

InFlightDiagnostic &InFlightDiagnostic::highlight(SrcRange R) {
  assert(IsActive && "Cannot modify an inactive diagnostic");
  if (Engine && R.isValid())
    Engine->getActiveDiagnostic().addRange(
        toCharSrcRange(Engine->SourceMgr, R));
  return *this;
}

InFlightDiagnostic &InFlightDiagnostic::highlightChars(SrcLoc Start,
                                                       SrcLoc End) {
  assert(IsActive && "Cannot modify an inactive diagnostic");
  if (Engine && Start.isValid())
    Engine->getActiveDiagnostic().addRange(
        toCharSrcRange(Engine->SourceMgr, Start, End));
  return *this;
}

/// Add an insertion fix-it to the currently-active diagnostic.  The
/// text is inserted immediately *after* the token specified.
///
InFlightDiagnostic &
InFlightDiagnostic::fixItInsertAfter(SrcLoc L, StringRef FormatString,
                                     ArrayRef<DiagnosticArgument> Args) {
  L = Engine->GetLexerBase()->GetLocForEndOfToken(Engine->SourceMgr, L);
  return fixItInsert(L, FormatString, Args);
}

/// Add a token-based removal fix-it to the currently-active
/// diagnostic.
InFlightDiagnostic &InFlightDiagnostic::fixItRemove(SrcRange R) {
  assert(IsActive && "Cannot modify an inactive diagnostic");
  if (R.isInvalid() || !Engine)
    return *this;

  // Convert from a token range to a CharSrcRange, which points to the end of
  // the token we want to remove.
  auto &SM = Engine->SourceMgr;
  auto charRange = toCharSrcRange(SM, R);

  // If we're removing something (e.g. a keyword), do a bit of extra work to
  // make sure that we leave the code in a good place, without extraneous white
  // space around its hole.  Specifically, check to see there is whitespace
  // before and after the end of range.  If so, nuke the space afterward to keep
  // things consistent.
  if (extractCharAfter(SM, charRange.getEnd()) == ' ' &&
      isspace(extractCharBefore(SM, charRange.getStart()))) {
    charRange =
        CharSrcRange(charRange.getStart(), charRange.getByteLength() + 1);
  }
  Engine->getActiveDiagnostic().addFixIt(Diagnostic::FixIt(charRange, {}, {}));
  return *this;
}

InFlightDiagnostic &
InFlightDiagnostic::fixItReplace(SrcRange R, StringRef FormatString,
                                 ArrayRef<DiagnosticArgument> Args) {
  auto &SM = Engine->SourceMgr;
  auto charRange = toCharSrcRange(SM, R);

  Engine->getActiveDiagnostic().addFixIt(
      Diagnostic::FixIt(charRange, FormatString, Args));
  return *this;
}

InFlightDiagnostic &InFlightDiagnostic::fixItReplace(SrcRange R,
                                                     StringRef Str) {
  if (Str.empty())
    return fixItRemove(R);

  assert(IsActive && "Cannot modify an inactive diagnostic");
  if (R.isInvalid() || !Engine)
    return *this;

  auto &SM = Engine->SourceMgr;
  auto charRange = toCharSrcRange(SM, R);

  // If we're replacing with something that wants spaces around it, do a bit of
  // extra work so that we don't suggest extra spaces.
  // FIXME: This could probably be applied to structured fix-its as well.
  if (Str.back() == ' ') {
    if (isspace(extractCharAfter(SM, charRange.getEnd())))
      Str = Str.drop_back();
  }
  if (!Str.empty() && Str.front() == ' ') {
    if (isspace(extractCharBefore(SM, charRange.getStart())))
      Str = Str.drop_front();
  }

  return fixItReplace(R, "%0", {Str});
}

InFlightDiagnostic &
InFlightDiagnostic::fixItReplaceChars(SrcLoc Start, SrcLoc End,
                                      StringRef FormatString,
                                      ArrayRef<DiagnosticArgument> Args) {
  assert(IsActive && "Cannot modify an inactive diagnostic");
  if (Engine && Start.isValid())
    Engine->getActiveDiagnostic().addFixIt(Diagnostic::FixIt(
        toCharSrcRange(Engine->SourceMgr, Start, End), FormatString, Args));
  return *this;
}

// SrcLoc
// DiagnosticEngine::getBestAddImportFixItLoc(const Decl *Member,
//                                            SourceFile *sourceFile) const {
//   auto &SM = SourceMgr;

//   SrcLoc bestLoc;

//   auto SF =
//       sourceFile ? sourceFile :
//       Member->getDeclContext()->getParentSourceFile();
//   if (!SF) {
//     return bestLoc;
//   }

//   for (auto item : SF->getTopLevelItems()) {
//     // If we found an import declaration, we want to insert after it.
//     if (auto importDecl =
//             dyn_cast_or_null<ImportDecl>(item.dyn_cast<Decl *>())) {
//       SrcLoc loc = importDecl->getEndLoc();
//       if (loc.isValid()) {
//         bestLoc = Lexer::getLocForEndOfLine(SM, loc);
//       }

//       // Keep looking for more import declarations.
//       continue;
//     }

//     // If we got a location based on import declarations, we're done.
//     if (bestLoc.isValid())
//       break;

//     // For any other item, we want to insert before it.
//     SrcLoc loc = item.getStartLoc();
//     if (loc.isValid()) {
//       bestLoc = Lexer::getLocForStartOfLine(SM, loc);
//       break;
//     }
//   }

//   return bestLoc;
// }

// InFlightDiagnostic &InFlightDiagnostic::fixItAddImport(StringRef
// ModuleName) {
//   assert(IsActive && "Cannot modify an inactive diagnostic");
//   auto Member = Engine->ActiveDiagnostic->getDecl();
//   SrcLoc bestLoc = Engine->getBestAddImportFixItLoc(Member);

//   if (bestLoc.isValid()) {
//     llvm::SmallString<64> importText;

//     // @_spi imports.
//     if (Member->isSPI()) {
//       auto spiGroups = Member->getSPIGroups();
//       if (!spiGroups.empty()) {
//         importText += "@_spi(";
//         importText += spiGroups[0].str();
//         importText += ") ";
//       }
//     }

//     importText += "import ";
//     importText += ModuleName;
//     importText += "\n";

//     return fixItInsert(bestLoc, importText);
//   }

//   return *this;
// }

InFlightDiagnostic &InFlightDiagnostic::fixItExchange(SrcRange R1,
                                                      SrcRange R2) {
  assert(IsActive && "Cannot modify an inactive diagnostic");

  auto &SM = Engine->SourceMgr;
  // Convert from a token range to a CharSrcRange
  auto charRange1 = toCharSrcRange(SM, R1);
  auto charRange2 = toCharSrcRange(SM, R2);
  // Extract source text.
  auto text1 = SM.extractText(charRange1);
  auto text2 = SM.extractText(charRange2);

  Engine->getActiveDiagnostic().addFixIt(
      Diagnostic::FixIt(charRange1, "%0", {text2}));
  Engine->getActiveDiagnostic().addFixIt(
      Diagnostic::FixIt(charRange2, "%0", {text1}));
  return *this;
}

InFlightDiagnostic &
InFlightDiagnostic::limitBehavior(DiagnosticBehavior limit) {
  Engine->getActiveDiagnostic().setBehaviorLimit(limit);
  return *this;
}

// InFlightDiagnostic &
// InFlightDiagnostic::limitBehaviorUntilStoneVersion(
//     DiagnosticBehavior limit, unsigned majorVersion) {
//   if (!Engine->languageVersion.isVersionAtLeast(majorVersion)) {
//     // If the behavior limit is a warning or less, wrap the diagnostic
//     // in a message that this will become an error in a later Stone
//     // version. We do this before limiting the behavior, because
//     // wrapIn will result in the behavior of the wrapping diagnostic.
//     if (limit >= DiagnosticBehavior::Warning)
//       wrapIn(error_in_future_swift_version, majorVersion);

//     limitBehavior(limit);
//   }

//   if (majorVersion == 6) {
//     if (auto stats = Engine->statsReporter) {
//       ++stats->getFrontendCounters().NumStone6Errors;
//     }
//   }

//   return *this;
// }

// InFlightDiagnostic &
// InFlightDiagnostic::warnUntilStoneVersion(unsigned majorVersion) {
//   return limitBehaviorUntilStoneVersion(DiagnosticBehavior::Warning,
//                                         majorVersion);
// }

// InFlightDiagnostic &
// InFlightDiagnostic::warnInStoneInterface(const DeclContext *context) {
//   auto sourceFile = context->getParentSourceFile();
//   if (sourceFile && sourceFile->Kind == SourceFileKind::Interface) {
//     return limitBehavior(DiagnosticBehavior::Warning);
//   }

//   return *this;
// }

InFlightDiagnostic &InFlightDiagnostic::wrapIn(const Diagnostic &wrapper) {
  // Save current active diagnostic into WrappedDiagnostics, ignoring state
  // so we don't get a None return or influence future diagnostics.
  DiagnosticState tempState;
  Engine->state.swap(tempState);
  llvm::SaveAndRestore<DiagnosticBehavior> limit(
      Engine->getActiveDiagnostic().BehaviorLimit,
      DiagnosticBehavior::Unspecified);

  Engine->WrappedDiagnostics.push_back(
      *Engine->diagnosticInfoForDiagnostic(Engine->getActiveDiagnostic()));

  Engine->state.swap(tempState);

  auto &wrapped = Engine->WrappedDiagnostics.back();

  // Copy and update its arg list.
  Engine->WrappedDiagnosticArgs.emplace_back(wrapped.FormatArgs);
  wrapped.FormatArgs = Engine->WrappedDiagnosticArgs.back();

  // Overwrite the ID and argument with those from the wrapper.
  Engine->getActiveDiagnostic().ID = wrapper.ID;
  Engine->getActiveDiagnostic().Args = wrapper.Args;

  // Set the argument to the diagnostic being wrapped.
  assert(wrapper.getArgs().front().getKind() ==
         DiagnosticArgumentKind::Diagnostic);
  Engine->getActiveDiagnostic().Args.front() = &wrapped;

  return *this;
}

void InFlightDiagnostic::FlushActiveDiagnostic() {
  if (!IsActive) {
    return;
  }

  IsActive = false;
  if (Engine) {
    Engine->flushActiveDiagnostic();
  }
}

void Diagnostic::addChildNote(Diagnostic &&D) {
  assert(storedDiagnosticInfos[(unsigned)D.ID].kind == DiagnosticKind::Note &&
         "Only notes can have a parent.");
  assert(storedDiagnosticInfos[(unsigned)ID].kind != DiagnosticKind::Note &&
         "Notes can't have children.");
  ChildNotes.push_back(std::move(D));
}

bool DiagnosticEngine::isDiagnosticPointsToFirstBadToken(DiagID ID) const {
  return storedDiagnosticInfos[(unsigned)ID].pointsToFirstBadToken;
}

bool DiagnosticEngine::isAPIDigesterBreakageDiagnostic(DiagID ID) const {
  return storedDiagnosticInfos[(unsigned)ID].isAPIDigesterBreakage;
}

bool DiagnosticEngine::isDeprecationDiagnostic(DiagID ID) const {
  return storedDiagnosticInfos[(unsigned)ID].isDeprecation;
}

bool DiagnosticEngine::isNoUsageDiagnostic(DiagID ID) const {
  return storedDiagnosticInfos[(unsigned)ID].isNoUsage;
}

bool DiagnosticEngine::finishProcessing() {
  bool hadError = false;
  for (auto &Consumer : Consumers) {
    hadError |= Consumer->finishProcessing();
  }
  return hadError;
}

StringRef stone::GetDiagnosticModifierString(DiagnosticModifier modifier) {
  switch (modifier) {
  case DiagnosticModifier::Error:
    return "error";
  case DiagnosticModifier::Select:
    return "select";
  case DiagnosticModifier::S:
    return "s";
  }
  llvm_unreachable("Unknown diagnostic modifier");
}

/// Skip forward to one of the given delimiters.
///
/// \param Text The text to search through, which will be updated to point
/// just after the delimiter.
///
/// \param Delim The first character delimiter to search for.
///
/// \param FoundDelim On return, true if the delimiter was found, or false
/// if the end of the string was reached.
///
/// \returns The string leading up to the delimiter, or the empty string
/// if no delimiter is found.
static StringRef skipToDelimiter(StringRef &Text, char Delim,
                                 bool *FoundDelim = nullptr) {
  unsigned Depth = 0;
  if (FoundDelim)
    *FoundDelim = false;

  unsigned I = 0;
  for (unsigned N = Text.size(); I != N; ++I) {
    if (Text[I] == '{') {
      ++Depth;
      continue;
    }
    if (Depth > 0) {
      if (Text[I] == '}')
        --Depth;
      continue;
    }

    if (Text[I] == Delim) {
      if (FoundDelim)
        *FoundDelim = true;
      break;
    }
  }

  assert(Depth == 0 && "Unbalanced {} set in diagnostic text");
  StringRef Result = Text.substr(0, I);
  Text = Text.substr(I + 1);
  return Result;
}

/// Handle the integer 'select' modifier.  This is used like this:
/// %select{foo|bar|baz}2.  This means that the integer argument "%2" has a
/// value from 0-2.  If the value is 0, the diagnostic prints 'foo'.
/// If the value is 1, it prints 'bar'.  If it has the value 2, it prints'baz'.
/// This is very useful for certain classes of variant diagnostics.
static void formatSelectionArgument(StringRef ModifierArguments,
                                    ArrayRef<DiagnosticArgument> Args,
                                    unsigned SelectedIndex,
                                    DiagnosticFormatOptions FormatOpts,
                                    llvm::raw_ostream &Out) {
  bool foundPipe = false;
  do {
    assert((!ModifierArguments.empty() || foundPipe) &&
           "Index beyond bounds in %select modifier");
    StringRef Text = skipToDelimiter(ModifierArguments, '|', &foundPipe);
    if (SelectedIndex == 0) {
      DiagnosticEngine::formatDiagnosticText(Out, Text, Args, FormatOpts);
      break;
    }
    --SelectedIndex;
  } while (true);
}

// static bool isInterestingTypealias(Type type) {
//   // Dig out the typealias declaration, if there is one.
//   TypeAliasDecl *aliasDecl = nullptr;
//   if (auto aliasTy = dyn_cast<TypeAliasType>(type.getPointer()))
//     aliasDecl = aliasTy->getDecl();
//   else
//     return false;

//   if (type->isVoid())
//     return false;

//   // The 'Stone.AnyObject' typealias is not 'interesting'.
//   if (aliasDecl->getName() ==
//         aliasDecl->getASTContext().getIdentifier("AnyObject") &&
//       (aliasDecl->getParentModule()->isStdlibModule() ||
//        aliasDecl->getParentModule()->isBuiltinModule())) {
//     return false;
//   }

//   // Compatibility aliases are only interesting insofar as their underlying
//   // types are interesting.
//   if (aliasDecl->isCompatibilityAlias()) {
//     auto underlyingTy = aliasDecl->getUnderlyingType();
//     return isInterestingTypealias(underlyingTy);
//   }

//   // Builtin types are never interesting typealiases.
//   if (type->is<BuiltinType>()) return false;

//   return true;
// }

// /// Walks the type recursively desugaring  types to display, but skipping
// /// `GenericTypeParamType` because we would lose association with its
// original
// /// declaration and end up presenting the parameter in τ_0_0 format on
// /// diagnostic.
// static Type getAkaTypeForDisplay(Type type) {
//   return type.transform([](Type visitTy) -> Type {
//     if (isa<SugarType>(visitTy.getPointer()) &&
//         !isa<GenericTypeParamType>(visitTy.getPointer()))
//       return getAkaTypeForDisplay(visitTy->getDesugaredType());
//     return visitTy;
//   });
// }

// /// Decide whether to show the desugared type or not.  We filter out some
// /// cases to avoid too much noise.
// static bool shouldShowAKA(Type type, StringRef typeName) {
//   // Canonical types are already desugared.
//   if (type->isCanonical())
//     return false;

//   // Only show 'aka' if there's a typealias involved; other kinds of sugar
//   // are easy enough for people to read on their own.
//   if (!type.findIf(isInterestingTypealias))
//     return false;

//   // If they are textually the same, don't show them.  This can happen when
//   // they are actually different types, because they exist in different
//   scopes
//   // (e.g. everyone names their type parameters 'T').
//   if (typeName == getAkaTypeForDisplay(type).getString())
//     return false;

//   return true;
// }

// /// If a type is part of an argument list which includes another, distinct
// type
// /// with the same string representation, it should be qualified during
// /// formatting.
// static bool typeSpellingIsAmbiguous(Type type,
//                                     ArrayRef<DiagnosticArgument> Args,
//                                     PrintOptions &PO) {
//   for (auto arg : Args) {
//     if (arg.getKind() == DiagnosticArgumentKind::Type) {
//       auto argType = arg.getAsType();
//       if (argType && argType->getWithoutParens().getPointer() !=
//       type.getPointer() &&
//           argType->getWithoutParens().getString(PO) == type.getString(PO)) {
//         // Currently, existential types are spelled the same way
//         // as protocols and compositions. We can remove this once
//         // existenials are printed with 'any'.
//         if (type->is<ExistentialType>() || argType->isExistentialType()) {
//           auto constraint = type;
//           if (auto existential = type->getAs<ExistentialType>())
//             constraint = existential->getConstraintType();

//           auto argConstraint = argType;
//           if (auto existential = argType->getAs<ExistentialType>())
//             argConstraint = existential->getConstraintType();

//           if (constraint.getPointer() != argConstraint.getPointer())
//             return true;

//           continue;
//         }

//         return true;
//       }
//     }
//   }
//   return false;
// }

// void swift::printClangDeclName(const clang::NamedDecl *ND,
//                                llvm::raw_ostream &os) {
//   ND->getNameForDiagnostic(os, ND->getASTContext().getPrintingPolicy(),
//   false);
// }

/// Format a single diagnostic argument and write it to the given
/// stream.
static void formatDiagnosticArgument(StringRef Modifier,
                                     StringRef ModifierArguments,
                                     ArrayRef<DiagnosticArgument> Args,
                                     unsigned ArgIndex,
                                     DiagnosticFormatOptions FormatOpts,
                                     llvm::raw_ostream &Out) {
  const DiagnosticArgument &Arg = Args[ArgIndex];
  switch (Arg.getKind()) {
  case DiagnosticArgumentKind::Integer:
    if (Modifier == "select") {
      assert(Arg.getAsInteger() >= 0 && "Negative selection index");
      formatSelectionArgument(ModifierArguments, Args, Arg.getAsInteger(),
                              FormatOpts, Out);
    } else if (Modifier == "s") {
      if (Arg.getAsInteger() != 1)
        Out << 's';
    } else {
      assert(Modifier.empty() && "Improper modifier for integer argument");
      Out << Arg.getAsInteger();
    }
    break;

  case DiagnosticArgumentKind::Unsigned:
    if (Modifier == "select") {
      formatSelectionArgument(ModifierArguments, Args, Arg.getAsUnsigned(),
                              FormatOpts, Out);
    } else if (Modifier == "s") {
      if (Arg.getAsUnsigned() != 1)
        Out << 's';
    } else {
      assert(Modifier.empty() && "Improper modifier for unsigned argument");
      Out << Arg.getAsUnsigned();
    }
    break;

  case DiagnosticArgumentKind::String:
    if (Modifier == "select") {
      formatSelectionArgument(ModifierArguments, Args,
                              Arg.getAsString().empty() ? 0 : 1, FormatOpts,
                              Out);
    } else {
      assert(Modifier.empty() && "Improper modifier for string argument");
      Out << Arg.getAsString();
    }
    break;

  case DiagnosticArgumentKind::Identifier:
    if (Modifier == "select") {
      formatSelectionArgument(ModifierArguments, Args,
                              Arg.getAsIdentifier().GetString().empty() ? 1 : 0,
                              FormatOpts, Out);
    } else {
      assert(Modifier.empty() && "Improper modifier for identifier argument");
      Out << FormatOpts.OpeningQuotationMark;
      // Arg.getAsIdentifier().printPretty(Out);
      Out << FormatOpts.ClosingQuotationMark;
    }
    break;

  case DiagnosticArgumentKind::Decl: {
    break;
  }
  case DiagnosticArgumentKind::VersionTuple: {
    assert(Modifier.empty() && "Improper modifier for VersionTuple argument");
    Out << Arg.getAsVersionTuple().getAsString();
    break;
  }

  case DiagnosticArgumentKind::Diagnostic: {
    assert(Modifier.empty() && "Improper modifier for Diagnostic argument");
    auto diagArg = Arg.getAsDiagnostic();
    DiagnosticEngine::formatDiagnosticText(Out, diagArg->FormatString,
                                           diagArg->FormatArgs);
    break;
  }
  case DiagnosticArgumentKind::ClangDecl: {
    assert(Modifier.empty() && "Improper modifier for ClangDecl argument");
    Out << FormatOpts.OpeningQuotationMark;
    printClangDeclName(Arg.getAsClangDecl(), Out);
    Out << FormatOpts.ClosingQuotationMark;
    break;
  }
  } // end switch
}

/// Format the given diagnostic text and place the result in the given
/// buffer.
void DiagnosticEngine::formatDiagnosticText(
    llvm::raw_ostream &Out, StringRef InText, ArrayRef<DiagnosticArgument> Args,
    DiagnosticFormatOptions FormatOpts) {
  while (!InText.empty()) {
    size_t Percent = InText.find('%');
    if (Percent == StringRef::npos) {
      // Write the rest of the string; we're done.
      Out.write(InText.data(), InText.size());
      break;
    }

    // Write the string up to (but not including) the %, then drop that text
    // (including the %).
    Out.write(InText.data(), Percent);
    InText = InText.substr(Percent + 1);

    // '%%' -> '%'.
    if (InText[0] == '%') {
      Out.write('%');
      InText = InText.substr(1);
      continue;
    }

    // Parse an optional modifier.
    StringRef Modifier;
    {
      size_t Length = InText.find_if_not(isalpha);
      Modifier = InText.substr(0, Length);
      InText = InText.substr(Length);
    }

    if (Modifier == "error") {
      Out << StringRef(
          "<<INTERNAL ERROR: encountered %error in diagnostic text>>");
      continue;
    }

    // Parse the optional argument list for a modifier, which is brace-enclosed.
    StringRef ModifierArguments;
    if (InText[0] == '{') {
      InText = InText.substr(1);
      ModifierArguments = skipToDelimiter(InText, '}');
    }

    // Find the digit sequence, and parse it into an argument index.
    size_t Length = InText.find_if_not(isdigit);
    unsigned ArgIndex;
    bool IndexParseFailed = InText.substr(0, Length).getAsInteger(10, ArgIndex);

    if (IndexParseFailed) {
      Out << StringRef(
          "<<INTERNAL ERROR: unparseable argument index in diagnostic text>>");
      continue;
    }

    InText = InText.substr(Length);

    if (ArgIndex >= Args.size()) {
      Out << StringRef(
          "<<INTERNAL ERROR: out-of-range argument index in diagnostic text>>");
      continue;
    }

    // Convert the argument to a string.
    formatDiagnosticArgument(Modifier, ModifierArguments, Args, ArgIndex,
                             FormatOpts, Out);
  }
}

static DiagnosticKind toDiagnosticKind(DiagnosticBehavior behavior) {
  switch (behavior) {
  case DiagnosticBehavior::Unspecified:
    llvm_unreachable("unspecified behavior");
  case DiagnosticBehavior::Ignore:
    llvm_unreachable("trying to map an ignored diagnostic");
  case DiagnosticBehavior::Error:
  case DiagnosticBehavior::Fatal:
    return DiagnosticKind::Error;
  case DiagnosticBehavior::Note:
    return DiagnosticKind::Note;
  case DiagnosticBehavior::Warning:
    return DiagnosticKind::Warning;
  case DiagnosticBehavior::Remark:
    return DiagnosticKind::Remark;
  }

  llvm_unreachable("Unhandled DiagnosticKind in switch.");
}

static DiagnosticBehavior toDiagnosticBehavior(DiagnosticKind kind,
                                               bool isFatal) {
  switch (kind) {
  case DiagnosticKind::Note:
    return DiagnosticBehavior::Note;
  case DiagnosticKind::Error:
    return isFatal ? DiagnosticBehavior::Fatal : DiagnosticBehavior::Error;
  case DiagnosticKind::Warning:
    return DiagnosticBehavior::Warning;
  case DiagnosticKind::Remark:
    return DiagnosticBehavior::Remark;
  }
  llvm_unreachable("Unhandled DiagnosticKind in switch.");
}

// A special option only for compiler writers that causes Diagnostics to assert
// when a failure diagnostic is emitted. Intended for use in the debugger.
llvm::cl::opt<bool> AssertOnError("swift-diagnostics-assert-on-error",
                                  llvm::cl::init(false));
// A special option only for compiler writers that causes Diagnostics to assert
// when a warning diagnostic is emitted. Intended for use in the debugger.
llvm::cl::opt<bool> AssertOnWarning("swift-diagnostics-assert-on-warning",
                                    llvm::cl::init(false));
DiagnosticBehavior DiagnosticState::determineBehavior(const Diagnostic &diag) {
  // We determine how to handle a diagnostic based on the following rules
  //   1) Map the diagnostic to its "intended" behavior, applying the behavior
  //      limit for this particular emission
  //   2) If current state dictates a certain behavior, follow that
  //   3) If the user ignored this specific diagnostic, follow that
  //   4) If the user substituted a different behavior for this behavior, apply
  //      that change
  //   5) Update current state for use during the next diagnostic

  //   1) Map the diagnostic to its "intended" behavior, applying the behavior
  //      limit for this particular emission
  auto diagInfo = storedDiagnosticInfos[(unsigned)diag.getID()];
  DiagnosticBehavior lvl =
      std::max(toDiagnosticBehavior(diagInfo.kind, diagInfo.isFatal),
               diag.getBehaviorLimit());
  assert(lvl != DiagnosticBehavior::Unspecified);

  //   2) If current state dictates a certain behavior, follow that

  // Notes relating to ignored diagnostics should also be ignored
  if (previousBehavior == DiagnosticBehavior::Ignore &&
      lvl == DiagnosticBehavior::Note)
    lvl = DiagnosticBehavior::Ignore;

  // Suppress diagnostics when in a fatal state, except for follow-on notes
  if (fatalErrorOccurred)
    if (!showDiagnosticsAfterFatalError && lvl != DiagnosticBehavior::Note)
      lvl = DiagnosticBehavior::Ignore;

  //   3) If the user ignored this specific diagnostic, follow that
  if (ignoredDiagnostics[(unsigned)diag.getID()])
    lvl = DiagnosticBehavior::Ignore;

  //   4) If the user substituted a different behavior for this behavior, apply
  //      that change
  if (lvl == DiagnosticBehavior::Warning) {
    if (warningsAsErrors)
      lvl = DiagnosticBehavior::Error;
    if (suppressWarnings)
      lvl = DiagnosticBehavior::Ignore;
  }

  if (lvl == DiagnosticBehavior::Remark) {
    if (suppressRemarks)
      lvl = DiagnosticBehavior::Ignore;
  }

  //   5) Update current state for use during the next diagnostic
  if (lvl == DiagnosticBehavior::Fatal) {
    fatalErrorOccurred = true;
    anyErrorOccurred = true;
  } else if (lvl == DiagnosticBehavior::Error) {
    anyErrorOccurred = true;
  }

  assert((!AssertOnError || !anyErrorOccurred) && "We emitted an error?!");
  assert((!AssertOnWarning || (lvl != DiagnosticBehavior::Warning)) &&
         "We emitted a warning?!");

  previousBehavior = lvl;
  return lvl;
}

void DiagnosticEngine::flushActiveDiagnostic() {
  assert(ActiveDiagnostic && "No active diagnostic to flush");
  handleDiagnostic(std::move(*ActiveDiagnostic));
  ActiveDiagnostic.reset();
}

void DiagnosticEngine::handleDiagnostic(Diagnostic &&diag) {
  if (TransactionCount == 0) {
    emitDiagnostic(diag);
    WrappedDiagnostics.clear();
    WrappedDiagnosticArgs.clear();
  } else {
    onTentativeDiagnosticFlush(diag);
    TentativeDiagnostics.emplace_back(std::move(diag));
  }
}

void DiagnosticEngine::clearTentativeDiagnostics() {
  TentativeDiagnostics.clear();
  WrappedDiagnostics.clear();
  WrappedDiagnosticArgs.clear();
}

void DiagnosticEngine::emitTentativeDiagnostics() {
  for (auto &diag : TentativeDiagnostics) {
    emitDiagnostic(diag);
  }
  clearTentativeDiagnostics();
}

void DiagnosticEngine::forwardTentativeDiagnosticsTo(
    DiagnosticEngine &targetEngine) {
  for (auto &diag : TentativeDiagnostics) {
    targetEngine.handleDiagnostic(std::move(diag));
  }
  clearTentativeDiagnostics();
}

/// Returns the access level of the least accessible PrettyPrintedDeclarations
/// buffer that \p decl should appear in.
///
/// This is always \c Public unless \p decl is a \c ValueDecl and its
/// access level is below \c Public. (That can happen with @testable and
/// @_private imports.)
// static VisibilityLevel getBufferVisibilityLevel(const Decl *decl) {
//   VisibilityLevel level = VisibilityLevel::Public;
//   if (auto *VD = dyn_cast<ValueDecl>(decl))
//     level = VD->getFormalVisibilityScope().accessLevelForDiagnostics();
//   if (level > VisibilityLevel::Public) level = VisibilityLevel::Public;
//   return level;
// }

static std::optional<DiagnosticInfo>
CreateDiagnosticInfoForDecl(const Diagnostic &diagnostic) {

  if (!diagnostic.IsDecl()) {
    return std::nullopt;
  }
  SrcLoc loc = diagnostic.getLoc();
  if (loc.isInvalid()) {
    const stone::Decl *decl = diagnostic.getDecl();

    // If a declaration was provided instead of a location, and that declaration
    // has a location we can point to, use that location.
    loc = decl->GetLoc();

    if (loc.isInvalid()) {
      // SrcLoc ppLoc = PrettyPrintedDeclarations[decl];
      // if (ppLoc.isInvalid()) {

      // }
    }
  }

  return DiagnosticInfo();
}
std::optional<DiagnosticInfo>
DiagnosticEngine::diagnosticInfoForDiagnostic(const Diagnostic &diagnostic) {

  auto behavior = state.determineBehavior(diagnostic);
  if (behavior == DiagnosticBehavior::Ignore) {
    return std::nullopt;
  }
  // Figure out the source location.
  SrcLoc loc = diagnostic.getLoc();
  if (loc.isInvalid() && diagnostic.IsDecl()) {
    const stone::Decl *decl = diagnostic.getDecl();
    // If a declaration was provided instead of a location, and that declaration
    // has a location we can point to, use that location.
    loc = decl->GetLoc();
  }

  if (loc.isInvalid()) {
  }

  auto fixIts = diagnostic.getFixIts();
  if (loc.isValid()) {
  }

  StringRef Category;

  return DiagnosticInfo(
      diagnostic.getID(), loc, toDiagnosticKind(behavior),
      diagnosticStringFor(diagnostic.getID(), getPrintDiagnosticNames()),
      diagnostic.getArgs(), Category, getDefaultDiagnosticLoc(),
      /*child note info*/ {}, diagnostic.getRanges(), fixIts,
      diagnostic.isChildNote());
}

void DiagnosticEngine::emitDiagnostic(const Diagnostic &diagnostic) {

  ArrayRef<Diagnostic> childNotes = diagnostic.getChildNotes();
  std::vector<Diagnostic> extendedChildNotes;

  if (auto info = diagnosticInfoForDiagnostic(diagnostic)) {
    // If the diagnostic location is within a buffer containing generated
    // source code, add child notes showing where the generation occurred.
    // We need to avoid doing this if this is itself a child note, as otherwise
    // we'd end up doubling up on notes.
    // if (!info->IsChildNote) {
    //    extendedChildNotes = getGeneratedSourceBufferNotes(info->Loc);
    // }

    if (!extendedChildNotes.empty()) {
      extendedChildNotes.insert(extendedChildNotes.end(), childNotes.begin(),
                                childNotes.end());
      childNotes = extendedChildNotes;
    }

    SmallVector<DiagnosticInfo, 1> childInfo;
    for (unsigned i : stone::indices(childNotes)) {
      auto child = diagnosticInfoForDiagnostic(childNotes[i]);
      assert(child);
      assert(child->Kind == DiagnosticKind::Note &&
             "Expected child diagnostics to all be notes?!");
      childInfo.push_back(*child);
    }
    TinyPtrVector<DiagnosticInfo *> childInfoPtrs;
    for (unsigned i : stone::indices(childInfo)) {
      childInfoPtrs.push_back(&childInfo[i]);
    }
    info->ChildDiagnosticInfo = childInfoPtrs;

    // SmallVector<std::string, 1> educationalNotePaths;
    // auto associatedNotes = educationalNotes[(uint32_t)diagnostic.getID()];
    // while (associatedNotes && *associatedNotes) {
    //   SmallString<128> notePath(getDiagnosticDocumentationPath());
    //   llvm::sys::path::append(notePath, *associatedNotes);
    //   educationalNotePaths.push_back(notePath.str().str());
    //   ++associatedNotes;
    // }
    // info->EducationalNotePaths = educationalNotePaths;

    // Now, pass it off to the consumers
    for (auto &consumer : Consumers) {
      consumer->handleDiagnostic(SourceMgr, *info);
    }
  }

  // For compatibility with DiagnosticConsumers which don't know about child
  // notes. These can be ignored by consumers which do take advantage of the
  // grouping.
  for (auto &childNote : childNotes) {
    emitDiagnostic(childNote);
  }
}

DiagnosticKind DiagnosticEngine::declaredDiagnosticKindFor(const DiagID id) {
  return storedDiagnosticInfos[(unsigned)id].kind;
}

llvm::StringRef
DiagnosticEngine::diagnosticStringFor(const DiagID id,
                                      bool printDiagnosticNames) {
  auto defaultMessage = printDiagnosticNames
                            ? debugDiagnosticStrings[(unsigned)id]
                            : diagnosticStrings[(unsigned)id];

  /// todo:
  // if (auto producer = localization.get()) {
  //   auto localizedMessage = producer->getMessageOr(id, defaultMessage);
  //   return localizedMessage;
  // }
  return defaultMessage;
}

llvm::StringRef DiagnosticEngine::diagnosticIDStringFor(const DiagID id) {
  return diagnosticIDStrings[(unsigned)id];
}

const char *InFlightDiagnostic::fixItStringFor(const FixItID id) {
  return fixItStrings[(unsigned)id];
}

void DiagnosticEngine::setBufferIndirectlyCausingDiagnosticToInput(SrcLoc loc) {
  // If in the future, nested BufferIndirectlyCausingDiagnosticRAII need be
  // supported, the compiler will need a stack for
  // bufferIndirectlyCausingDiagnostic.
  assert(bufferIndirectlyCausingDiagnostic.isInvalid() &&
         "Buffer should not already be set.");
  bufferIndirectlyCausingDiagnostic = loc;
  assert(bufferIndirectlyCausingDiagnostic.isValid() &&
         "Buffer must be valid for previous assertion to work.");
}

void DiagnosticEngine::resetBufferIndirectlyCausingDiagnostic() {
  bufferIndirectlyCausingDiagnostic = SrcLoc();
}

DiagnosticSuppression::DiagnosticSuppression(DiagnosticEngine &diags)
    : diags(diags) {
  consumers = diags.takeConsumers();
}

DiagnosticSuppression::~DiagnosticSuppression() {
  for (auto consumer : consumers) {
    diags.addConsumer(*consumer);
  }
}

bool DiagnosticSuppression::isEnabled(const DiagnosticEngine &diags) {
  return diags.getConsumers().empty();
}

// BufferIndirectlyCausingDiagnosticRAII::BufferIndirectlyCausingDiagnosticRAII(
//     const SourceFile &SF)
//     : Diags(SF.GetASTContext().GetDiags()) {

//   if (SF.HasBufferID()) {
//     auto loc =
//         SF.GetASTContext().GetSrcMgr().getLocForBufferStart(SF.GetBufferID());
//     if (loc.isValid()) {
//       Diags.setBufferIndirectlyCausingDiagnosticToInput(loc);
//     }
//   }
// }

void DiagnosticEngine::onTentativeDiagnosticFlush(Diagnostic &diagnostic) {
  for (auto &argument : diagnostic.Args) {
    if (argument.getKind() != DiagnosticArgumentKind::String)
      continue;

    auto content = argument.getAsString();
    if (content.empty())
      continue;

    auto I = TransactionStrings.insert(content).first;
    argument = DiagnosticArgument(StringRef(I->getKeyData()));
  }
}

// EncodedDiagnosticMessage::EncodedDiagnosticMessage(StringRef S)
//     : Message(Lexer::getEncodedStringSegment(S, Buf, /*IsFirstSegment=*/true,
//                                              /*IsLastSegment=*/true,
//                                              /*IndentToStrip=*/~0U)) {}

void stone::printClangDeclName(const clang::NamedDecl *namedDecl,
                               llvm::raw_ostream &os) {
  // TODO:
  // namedDecl->getNameForDiagnostic(os,
  // namedDecl->getASTContext().getPrintingPolicy(), false);
}

DiagnosticConsumer::~DiagnosticConsumer() = default;

llvm::SMLoc DiagnosticConsumer::getRawLoc(SrcLoc loc) { return loc.Value; }

FixIt::FixIt(CharSrcRange R, StringRef Str, ArrayRef<DiagnosticArgument> Args)
    : Range(R) {
  // FIXME: Defer text formatting to later in the pipeline.
  llvm::raw_string_ostream OS(Text);
  DiagnosticEngine::formatDiagnosticText(
      OS, Str, Args, DiagnosticFormatOptions::formatForFixIts());
}

// These must come after the declaration of AnnotatedSourceSnippet due to the
// `currentSnippet` member.
TextDiagnosticPrinter::TextDiagnosticPrinter(llvm::raw_ostream &stream)
    : Stream(stream) {}

TextDiagnosticPrinter::~TextDiagnosticPrinter() {}

bool TextDiagnosticPrinter::finishProcessing() {
  // If there's an in-flight snippet, flush it.
  flush();
  return false;
}

namespace {
class ColoredStream : public raw_ostream {
  raw_ostream &Underlying;

public:
  explicit ColoredStream(raw_ostream &underlying) : Underlying(underlying) {}
  ~ColoredStream() override { flush(); }

  raw_ostream &changeColor(Colors color, bool bold = false,
                           bool bg = false) override {
    Underlying.changeColor(color, bold, bg);
    return *this;
  }
  raw_ostream &resetColor() override {
    Underlying.resetColor();
    return *this;
  }
  raw_ostream &reverseColor() override {
    Underlying.reverseColor();
    return *this;
  }
  bool has_colors() const override { return true; }

  void write_impl(const char *ptr, size_t size) override {
    Underlying.write(ptr, size);
  }
  uint64_t current_pos() const override {
    return Underlying.tell() - GetNumBytesInBuffer();
  }

  size_t preferred_buffer_size() const override { return 0; }
};

} // namespace

void TextDiagnosticPrinter::PrintDiagnosticWithStoneFormattingStyle(
    SrcMgr &SM, const DiagnosticInfo &Info) {
  assert(HasDiagnosticFormatter() &&
         "Compiler formatting requires a diagnostic-formatter!");

  // Display the diagnostic.
  ColoredStream coloredErrs{Stream};
  llvm::raw_ostream &out = ForceColors ? coloredErrs : Stream;

  // (1) Format the message
  llvm::SmallString<256> Text;
  {
    llvm::raw_svector_ostream Out(Text);
    GetDiagnosticFormatter()->FormatDiagnosticText(Out, Info.FormatString,
                                                   Info.FormatArgs);
  }

  // (2) Print the message
}

void TextDiagnosticPrinter::PrintDiagnosticWithLLVMFormattingStyle(
    SrcMgr &SM, const DiagnosticInfo &Info) {
  // Determine what kind of diagnostic we're emitting.
  llvm::SourceMgr::DiagKind SMKind;
  switch (Info.Kind) {
  case DiagnosticKind::Error:
    SMKind = llvm::SourceMgr::DK_Error;
    break;
  case DiagnosticKind::Warning:
    SMKind = llvm::SourceMgr::DK_Warning;
    break;

  case DiagnosticKind::Note:
    SMKind = llvm::SourceMgr::DK_Note;
    break;

  case DiagnosticKind::Remark:
    SMKind = llvm::SourceMgr::DK_Remark;
    break;
  }

  // Translate ranges.
  SmallVector<llvm::SMRange, 2> Ranges;
  for (auto R : Info.Ranges) {
    Ranges.push_back(getRawRange(SM, R));
  }

  // Translate fix-its.
  SmallVector<llvm::SMFixIt, 2> FixIts;
  for (FixIt F : Info.FixIts) {
    FixIts.push_back(getRawFixIt(SM, F));
  }

  // Display the diagnostic.
  ColoredStream coloredErrs{Stream};
  raw_ostream &out = ForceColors ? coloredErrs : Stream;
  const llvm::SourceMgr &rawSM = SM.GetLLVMSrcMgr();

  // Actually substitute the diagnostic arguments into the diagnostic text.
  llvm::SmallString<256> Text;
  {
    llvm::raw_svector_ostream Out(Text);

    DiagnosticEngine::formatDiagnosticText(Out, Info.FormatString,
                                           Info.FormatArgs);
  }

  auto Msg = SM.GetMessage(Info.Loc, SMKind, Text, Ranges, FixIts,
                           EmitMacroExpansionFiles);
  rawSM.PrintMessage(out, Msg, ForceColors);
}

void TextDiagnosticPrinter::handleDiagnostic(SrcMgr &SM,
                                             const DiagnosticInfo &Info,
                                             DiagnosticEngine *CB) {
  if (Info.Kind == DiagnosticKind::Error) {
    DidErrorOccur = true;
  }
  if (SuppressOutput) {
    return;
  }
  if (Info.IsChildNote) {
    return;
  }
  switch (FormattingStyle) {
  case DiagnosticOptions::FormattingStyle::Stone: {
    PrintDiagnosticWithStoneFormattingStyle(SM, Info);
    break;
  }
  case DiagnosticOptions::FormattingStyle::LLVM: {
    PrintDiagnosticWithLLVMFormattingStyle(SM, Info);
    break;
  }
  default: {
    assert(false && "Unknown formatting sytle -- unable to print diagnostic!");
  }
  }
}

void TextDiagnosticPrinter::flush(bool includeTrailingBreak) {}
