#ifndef STONE_DIAG_DIAGNOSTICFORMATLEXER_H
#define STONE_DIAG_DIAGNOSTICFORMATLEXER_H

#include "stone/Basic/SrcMgr.h"
#include "stone/Basic/Token.h"
#include "stone/Support/Trivia.h"

// TODO: Move to support
namespace stone {
class SrcMgr;
class Token;

namespace diag {
/// Given a pointer to the starting byte of a UTF8 character, validate it and
/// advance the lexer past it.  This returns the encoded character or ~0U if
/// the encoding is invalid.
uint32_t validateUTF8CharacterAndAdvance(const char *&Ptr, const char *End);

// template <typename... T> struct Diag;

enum class CommentRetentionMode {
  None,
  AttachToNextToken,
  ReturnAsTokens,
};

enum class TriviaRetentionMode {
  WithoutTrivia,
  WithTrivia,
};

enum class HashbangMode : bool {
  Disallowed,
  Allowed,
};

enum class LexerMode {
  Stone,
  StoneInterface,
};

/// Kinds of conflict marker which the lexer might encounter.
enum class ConflictMarkerKind {
  /// A normal or diff3 conflict marker, initiated by at least 7 "<"s,
  /// separated by at least 7 "="s or "|"s, and terminated by at least 7 ">"s.
  Normal,
  /// A Perforce-style conflict marker, initiated by 4 ">"s,
  /// separated by 4 "="s, and terminated by 4 "<"s.
  Perforce
};

class LexerState final {
  friend class DiagnosticTextLexer;

public:
  LexerState() {}
  bool IsValid() const { return loc.isValid(); }

  LexerState Advance(unsigned offset) const {
    assert(IsValid());
    return LexerState(loc.getAdvancedLoc(offset));
  }

private:
  explicit LexerState(SrcLoc loc) : loc(loc) {}
  SrcLoc loc;
  llvm::StringRef leadingTrivia;
  friend class Lexer;
};

// TODO: ParsingOptions
class DiagnosticTextLexer final {

  const unsigned BufferID;
  const SrcMgr &sm;
  llvm::raw_ostream &Diag;
  LexerState state;

  /// Pointer to the first character of the buffer, even in a lexer that
  /// scans a subrange of the buffer.
  const char *BufferStart;

  /// Pointer to one past the end character of the buffer, even in a lexer
  /// that scans a subrange of the buffer.  Because the buffer is always
  /// NUL-terminated, this points to the NUL terminator.
  const char *BufferEnd;

  /// Pointer to the artificial EOF that is located before BufferEnd.  Useful
  /// for lexing subranges of a buffer.
  const char *ArtificialEOF = nullptr;

  /// If non-null, points to the '\0' character in the buffer where we should
  /// produce a code completion token.
  const char *CodeCompletionPtr = nullptr;

  /// Points to BufferStart or past the end of UTF-8 BOM sequence if it exists.
  const char *ContentStart;

  /// Pointer to the next not consumed character.
  const char *CurPtr;

  Token NextToken;

  /// The kind of source we're lexing. This either enables special behavior for
  /// module interfaces, or enables things like the 'sil' keyword if lexing
  /// a .sil file.
  const LexerMode LexMode;

  /// True if we should skip past a `#!` line at the start of the file.
  const bool IsHashbangAllowed;

  const CommentRetentionMode RetainComments;

  const TriviaRetentionMode TriviaRetention;

  /// The current leading trivia for the next token.
  ///
  /// The StringRef points into the source buffer that is currently being lexed.
  llvm::StringRef LeadingTrivia;

  /// The current trailing trivia for the next token.
  /// The StringRef points into the source buffer that is currently being lexed.
  llvm::StringRef TrailingTrivia;

  /// The location at which the comment of the next token starts. \c nullptr if
  /// the next token doesn't have a comment.
  const char *CommentStart;

  /// If this is not \c nullptr, all tokens after this point are treated as eof.
  /// Used to cut off lexing early when we detect that the nesting level is too
  /// deep.
  const char *LexerCutOffPoint = nullptr;

  DiagnosticTextLexer(const DiagnosticTextLexer &) = delete;
  void operator=(const DiagnosticTextLexer &) = delete;

  struct PrincipalCtor {};

  /// The principal constructor used by public constructors below.
  /// Don't use this constructor for other purposes, it does not initialize
  /// everything.
  DiagnosticTextLexer(const PrincipalCtor &, unsigned BufferID,
                      const SrcMgr &sm, llvm::raw_ostream &Diag,
                      LexerMode LexMode, HashbangMode HashbangAllowed,
                      CommentRetentionMode RetainComments,
                      TriviaRetentionMode TriviaRetention);

  void Lex();
  void initialize(unsigned Offset, unsigned EndOffset);

public:
  //=Lexer options goes here=/
  bool warnOnEditorPlaceholder = false;

  static constexpr unsigned extraIndentationSize = 4;

public:
  /// Create a normal lexer that scans the whole source buffer.
  ///
  /// \param Options - the language options under which to lex.  By
  ///   design, language options only affect whether a token is valid
  ///   and/or the exact token kind produced (e.g. keyword or
  ///   identifier), but not things like how many characters are
  ///   consumed.  If that changes, APIs like GetLocForEndOfToken will
  ///   need to take a CompilerOptions explicitly.
  /// \param LexMode - the kind of source file we're lexing.
  ///   Unlike language options, this does affect primitive lexing, which
  ///   means that APIs like GetLocForEndOfToken really ought to take
  ///   this flag; it's just that we don't care that much about fidelity
  ///   when parsing SIL files.
  DiagnosticTextLexer(
      unsigned BufferID, const SrcMgr &sm, llvm::raw_ostream &Diag,
      LexerMode LexMode,
      HashbangMode HashbangAllowed = HashbangMode::Disallowed,
      CommentRetentionMode RetainComments = CommentRetentionMode::None,
      TriviaRetentionMode TriviaRetention = TriviaRetentionMode::WithoutTrivia);

  DiagnosticTextLexer(unsigned BufferID, const stone::SrcMgr &SM,
                      llvm::raw_ostream &Diag);

  /// Create a lexer that scans a subrange of the source buffer.
  DiagnosticTextLexer(unsigned BufferID, const stone::SrcMgr &sm,
                      llvm::raw_ostream &Diag, LexerMode LexMode,
                      HashbangMode HashbangAllowed,
                      CommentRetentionMode RetainComments,
                      TriviaRetentionMode TriviaRetention, unsigned Offset,
                      unsigned EndOffset);

  /// Create a sub-lexer that lexes from the same buffer, but scans
  /// a subrange of the buffer.
  ///
  /// \param Parent the parent lexer that scans the whole buffer
  /// \param BeginState start of the subrange
  /// \param EndState end of the subrange
  DiagnosticTextLexer(DiagnosticTextLexer &Parent, LexerState BeginState,
                      LexerState EndState);

  /// Returns true if this lexer will produce a code completion token.
  bool isCodeCompletion() const { return CodeCompletionPtr != nullptr; }

  /// Whether we are lexing a Swift interface file.
  bool IsStoneInterface() const { return LexMode == LexerMode::StoneInterface; }

  /// Lex a token. If \c TriviaRetentionMode is \c WithTrivia, passed pointers
  /// to trivias are populated.
  void Lex(Token &Result, llvm::StringRef &LeadingTriviaResult,
           llvm::StringRef &TrailingTriviaResult) {
    Result = NextToken;
    if (TriviaRetention == TriviaRetentionMode::WithTrivia) {
      LeadingTriviaResult = LeadingTrivia;
      TrailingTriviaResult = TrailingTrivia;
    }
    if (Result.IsNot(tok::eof))
      Lex();
  }

  void Lex(Token &Result) {
    llvm::StringRef LeadingTrivia, TrailingTrivia;
    Lex(Result, LeadingTrivia, TrailingTrivia);
  }

  /// Reset the lexer's buffer pointer to \p Offset bytes after the buffer
  /// start.
  void resetToOffset(size_t Offset) {
    assert(BufferStart + Offset <= BufferEnd && "Offset after buffer end");
    CurPtr = BufferStart + Offset;
    Lex();
  }

  /// Cut off lexing at the current position. The next token to be lexed will
  /// be an EOF token, even if there is still source code to be lexed.
  /// The current and next token (returned by \c Peek ) are not
  /// modified. The token after \c NextToken will be the EOF token.
  void cutOffLexing() {
    // If we already have a cut off point, don't push it further towards the
    // back.
    if (LexerCutOffPoint == nullptr || LexerCutOffPoint >= CurPtr) {
      LexerCutOffPoint = CurPtr;
    }
  }

  /// If a lexer cut off point has been set returns the offset in the buffer at
  /// which lexing is being cut off.
  std::optional<size_t> lexingCutOffOffset() const {
    if (LexerCutOffPoint) {
      return LexerCutOffPoint - BufferStart;
    } else {
      return std::nullopt;
    }
  }

  bool isKeepingComments() const {
    return RetainComments == CommentRetentionMode::ReturnAsTokens;
  }

  unsigned getBufferID() const { return BufferID; }

  /// Peek - Return the next token to be returned by Lex without
  /// actually lexing it.
  const Token &Peek() const { return NextToken; }

  /// Returns the lexer state for the beginning of the given token
  /// location. After restoring the state, lexer will return this token and
  /// continue from there.
  LexerState getStateForBeginningOfTokenLoc(SrcLoc Loc) const;

  /// Returns the lexer state for the beginning of the given token.
  /// After restoring the state, lexer will return this token and continue from
  /// there.
  LexerState
  getStateForBeginningOfToken(const Token &Tok,
                              const StringRef &LeadingTrivia = {}) const {

    // If the token has a comment attached to it, rewind to before the comment,
    // not just the start of the token.  This ensures that we will re-lex and
    // reattach the comment to the token if rewound to this state.
    SrcLoc TokStart = Tok.GetCommentStart();
    if (TokStart.isInvalid())
      TokStart = Tok.GetLoc();
    auto S = getStateForBeginningOfTokenLoc(TokStart);
    if (TriviaRetention == TriviaRetentionMode::WithTrivia) {
      S.leadingTrivia = LeadingTrivia;
    } else {
      S.leadingTrivia = StringRef();
    }
    return S;
  }

  LexerState getStateForEndOfTokenLoc(SrcLoc Loc) const {
    return LexerState(GetLocForEndOfTokenImpl(sm, Loc));
  }

  bool isStateForCurrentBuffer(LexerState state) const {
    return sm.findBufferContainingLoc(state.loc) == getBufferID();
  }

  /// Restore the lexer state to a given one, that can be located either
  /// before or after the current position.
  void restoreState(LexerState S, bool enableDiagnostics = false) {
    assert(S.IsValid());
    CurPtr = getBufferPtrForSrcLoc(S.loc);

    // Don't reemit diagnostics while readvancing the lexer.
    // llvm::SaveAndRestore<llvm::raw_ostream *> DE(
    //     de, enableDiagnostics ? de : nullptr);

    Lex();
    // Restore Trivia.
    if (TriviaRetention == TriviaRetentionMode::WithTrivia)
      LeadingTrivia = S.leadingTrivia;
  }

  /// Restore the lexer state to a given state that is located before
  /// current position.
  void backtrackToState(LexerState S) {
    assert(getBufferPtrForSrcLoc(S.loc) <= CurPtr && "can't backtrack forward");
    restoreState(S);
  }

  /// Retrieve the Token referred to by \c Loc.
  ///
  /// \param SM The source manager in which the given source location
  /// resides.
  ///
  /// \param Loc The source location of the beginning of a token.
  ///
  /// \param CRM How comments should be treated by the lexer. Default is to
  /// return the comments as tokens. This is needed in situations where
  /// detecting the next semantically meaningful token is required, such as
  /// the 'implicit self' diagnostic determining whether a capture list is
  /// empty (i.e., the opening bracket is immediately followed by a closing
  /// bracket, possibly with comments in between) in order to insert the
  /// appropriate fix-it.
  static Token getTokenAtLocation(
      const stone::SrcMgr &SM, SrcLoc Loc,
      CommentRetentionMode CRM = CommentRetentionMode::ReturnAsTokens);

  /// Retrieve the source location that points just past the
  /// end of the token referred to by \c Loc.
  ///
  /// \param SM The source manager in which the given source location
  /// resides.
  ///
  /// \param Loc The source location of the beginning of a token.
  static SrcLoc GetLocForEndOfTokenImpl(const stone::SrcMgr &SM, SrcLoc Loc);

  /// Convert a SrcRange to the equivalent CharSrcRange
  ///
  /// \param SM The source manager in which the given source range
  /// resides.
  ///
  /// \param SR The source range
  static CharSrcRange getCharSrcRangeFromSrcRange(const stone::SrcMgr &SM,
                                                  const SrcRange &SR) {
    return CharSrcRange(SM, SR.Start, GetLocForEndOfTokenImpl(SM, SR.End));
  }

  /// Return the start location of the token that the offset in the given buffer
  /// points to.
  ///
  /// Note that this is more expensive than \c GetLocForEndOfToken because it
  /// finds and re-lexes from the beginning of the line.
  ///
  /// Due to the parser splitting tokens the adjustment may be incorrect, e.g:
  /// \code
  ///   func +<T>(a : T, b : T)
  /// \endcode
  /// The start of the '<' token is '<', but the lexer will produce "+<" before
  /// the parser splits it up.
  ////
  /// If the offset points to whitespace the returned source location will point
  /// to the whitespace offset.
  static SrcLoc GetLocForStartOfToken(stone::SrcMgr &SM, unsigned BufferID,
                                      unsigned Offset);

  static SrcLoc GetLocForStartOfToken(stone::SrcMgr &SM, SrcLoc Loc);

  /// Retrieve the start location of the line containing the given location.
  /// the given location.
  static SrcLoc GetLocForStartOfLine(stone::SrcMgr &SM, SrcLoc Loc);

  /// Retrieve the source location for the end of the line containing the
  /// given location, which is the location of the start of the next line.
  static SrcLoc GetLocForEndOfLine(stone::SrcMgr &SM, SrcLoc Loc);

  /// Retrieve the string used to indent the line that contains the given
  /// source location.
  ///
  /// If \c ExtraIndentation is not null, it will be set to an appropriate
  /// additional intendation for adding code in a smaller scope "within" \c Loc.
  static StringRef getIndentationForLine(stone::SrcMgr &SM, SrcLoc Loc,
                                         StringRef *ExtraIndentation = nullptr);

  /// Determines if the given string is a valid non-operator
  /// identifier, without escaping characters.
  static bool isIdentifier(llvm::StringRef identifier);

  /// Determine the token kind of the string, given that it is a valid
  /// non-operator identifier. Return tok::identifier if the string is not a
  /// reserved word.
  static tok kindOfIdentifier(llvm::StringRef identifier);

  /// Determines if the given string is a valid operator identifier,
  /// without escaping characters.
  static bool isOperator(llvm::StringRef operatorStr);

  SrcLoc GetLocForStartOfBuffer() const {
    return SrcLoc(SrcLoc::GetFromPtr(BufferStart));
  }

  /// StringSegment - A segment of a (potentially interpolated) string.
  struct StringSegment {
    enum : char { Literal, Expr } Kind;
    // Loc+Length for the segment inside the string literal, without quotes.
    SrcLoc Loc;
    unsigned Length, IndentToStrip, CustomDelimiterLen;
    bool IsFirstSegment, IsLastSegment;

    static StringSegment getLiteral(SrcLoc Loc, unsigned Length,
                                    bool IsFirstSegment, bool IsLastSegment,
                                    unsigned IndentToStrip,
                                    unsigned CustomDelimiterLen) {
      StringSegment Result;
      Result.Kind = Literal;
      Result.Loc = Loc;
      Result.Length = Length;
      Result.IsFirstSegment = IsFirstSegment;
      Result.IsLastSegment = IsLastSegment;
      Result.IndentToStrip = IndentToStrip;
      Result.CustomDelimiterLen = CustomDelimiterLen;
      return Result;
    }

    static StringSegment getExpr(SrcLoc Loc, unsigned Length) {
      StringSegment Result;
      Result.Kind = Expr;
      Result.Loc = Loc;
      Result.Length = Length;
      Result.IsFirstSegment = false;
      Result.IsLastSegment = false;
      Result.IndentToStrip = 0;
      Result.CustomDelimiterLen = 0;
      return Result;
    }

    SrcLoc getEndLoc() { return Loc.getAdvancedLoc(Length); }
  };

  /// Implementation of getEncodedStringSegment. Note that \p Str must support
  /// reading one byte past the end.
  static StringRef getEncodedStringSegmentImpl(
      StringRef Str, SmallVectorImpl<char> &Buffer, bool IsFirstSegment,
      bool IsLastSegment, unsigned IndentToStrip, unsigned CustomDelimiterLen);

  /// Compute the bytes that the actual string literal should codegen to.
  /// If a copy needs to be made, it will be allocated out of the provided
  /// \p Buffer.
  StringRef getEncodedStringSegment(StringSegment Segment,
                                    SmallVectorImpl<char> &Buffer) const {
    return getEncodedStringSegmentImpl(
        StringRef(getBufferPtrForSrcLoc(Segment.Loc), Segment.Length), Buffer,
        Segment.IsFirstSegment, Segment.IsLastSegment, Segment.IndentToStrip,
        Segment.CustomDelimiterLen);
  }

  /// Given a string encoded with escapes like a string literal, compute
  /// the byte content.
  ///
  /// If a copy needs to be made, it will be allocated out of the provided
  /// \p Buffer. If \p IndentToStrip is '~0U', the indent is auto-detected.
  static llvm::StringRef getEncodedStringSegment(
      StringRef Str, SmallVectorImpl<char> &Buffer, bool IsFirstSegment = false,
      bool IsLastSegment = false, unsigned IndentToStrip = 0,
      unsigned CustomDelimiterLen = 0) {
    SmallString<128> TerminatedStrBuf(Str);
    TerminatedStrBuf.push_back('\0');
    StringRef TerminatedStr = StringRef(TerminatedStrBuf).drop_back();
    StringRef Result = getEncodedStringSegmentImpl(
        TerminatedStr, Buffer, IsFirstSegment, IsLastSegment, IndentToStrip,
        CustomDelimiterLen);
    if (Result == TerminatedStr)
      return Str;
    assert(Result.data() == Buffer.data());
    return Result;
  }

  /// Given a string literal token, separate it into string/expr segments
  /// of a potentially interpolated string.
  static void getStringLiteralSegments(const Token &Str,
                                       SmallVectorImpl<StringSegment> &Segments,
                                       llvm::raw_ostream &Diag);

  void getStringLiteralSegments(const Token &Str,
                                SmallVectorImpl<StringSegment> &Segments) {
    return getStringLiteralSegments(Str, Segments, Diag);
  }

  static SrcLoc getSrcLoc(const char *Loc) { return SrcLoc::GetFromPtr(Loc); }

  /// Get the token that starts at the given location.
  Token getTokenAt(SrcLoc Loc);

private:
  /// Nul character meaning kind.
  enum class NullCharacterKind {
    /// String buffer terminator.
    BufferEnd,
    /// Embedded nul character.
    Embedded,
    /// Code completion marker.
    CodeCompletion
  };

  /// For a source location in the current buffer, returns the corresponding
  /// pointer.
  const char *getBufferPtrForSrcLoc(SrcLoc Loc) const {
    return BufferStart + sm.getLocOffsetInBuffer(Loc, BufferID);
  }

  void formToken(tok Kind, const char *TokStart);
  void formEscapedIdentifierToken(const char *TokStart);
  void formStringLiteralToken(const char *TokStart, bool IsMultilineString,
                              unsigned CustomDelimiterLen);

  /// Advance to the end of the line.
  /// If EatNewLine is true, CurPtr will be at end of newline character.
  /// Otherwise, CurPtr will be at newline character.
  void skipToEndOfLine(bool EatNewline);

  /// Skip to the end of the line of a // comment.
  void skipSlashSlashComment(bool EatNewline);

  /// Skip a #! hashbang line.
  void skipHashbang(bool EatNewline);

  void skipSlashStarComment();

  void lexHash();
  void lexIdentifier();
  void lexDollarIdent();
  void lexOperatorIdentifier();
  void lexHexNumber();
  void lexNumber();

  /// Skip over trivia and return characters that were skipped over in a \c
  /// StringRef. \p AllTriviaStart determines the start of the trivia. In nearly
  /// all cases, this should be \c CurPtr. If other trivia has already been
  /// skipped over (like a BOM), this can be used to point to the start of the
  /// BOM. The returned \c StringRef will always start at \p AllTriviaStart.
  StringRef lexTrivia(bool IsForTrailingTrivia, const char *AllTriviaStart);
  static unsigned lexUnicodeEscape(const char *&CurPtr,
                                   DiagnosticTextLexer *Diags);

  unsigned lexCharacter(const char *&CurPtr, char StopQuote,
                        bool EmitDiagnostics, bool IsMultilineString = false,
                        unsigned CustomDelimiterLen = 0);
  void lexStringLiteral(unsigned CustomDelimiterLen = 0);
  void lexEscapedIdentifier();

  void lexRegexLiteral(const char *TokStart);

  void tryLexEditorPlaceholder();
  const char *findEndOfCurlyQuoteStringLiteral(const char *,
                                               bool EmitDiagnostics);

  /// Try to lex conflict markers by checking for the presence of the start and
  /// end of the marker in diff3 or Perforce style respectively.
  bool tryLexConflictMarker(bool EatNewline);

  /// Returns it should be tokenize.
  bool lexUnknown(bool EmitDiagnosticsIfToken);

  NullCharacterKind getNullCharacterKind(const char *Ptr) const;

  /// Emit diagnostics for single-quote string and suggest replacement
  /// with double-quoted equivalent.
  ///
  /// Or, if we're in strawperson mode, we will emit a custom
  /// error message instead, determined by the Swift library.
  void diagnoseSingleQuoteStringLiteral(const char *TokStart,
                                        const char *TokEnd);
};

/// A lexer that can lex trivia into its pieces
class TriviaLexer {
public:
  /// Decompose the triva in \p TriviaStr into their pieces.
  static Trivia lexTrivia(StringRef TriviaStr);
};

/// Given an ordered token \param Array , get the iterator pointing to the first
/// token that is not before \param Loc .
template <typename ArrayTy, typename Iterator = typename ArrayTy::iterator>
Iterator token_lower_bound(ArrayTy &Array, SrcLoc Loc) {
  return std::lower_bound(
      Array.begin(), Array.end(), Loc, [](const Token &T, SrcLoc L) {
        return T.GetLoc().getOpaquePointerValue() < L.getOpaquePointerValue();
      });
}

/// Given an ordered token array \param AllTokens , get the slice of the array
/// where front() locates at \param StartLoc and back() locates at \param EndLoc
/// .
llvm::ArrayRef<Token> slice_token_array(ArrayRef<Token> AllTokens,
                                        SrcLoc StartLoc, SrcLoc EndLoc);

} // namespace diag
} // namespace stone

#endif