#ifndef STONE_PARSE_LEXER_H
#define STONE_PARSE_LEXER_H

#include "stone/AST/Diagnostics.h"
#include "stone/Basic/Token.h"
#include "stone/Support/LexerBase.h"
#include "stone/Support/Statistics.h"

// TODO: Move to support
namespace stone {
class SrcMgr;
class Token;
/// Given a pointer to the starting byte of a UTF8 character, validate it and
/// advance the lexer past it.  This returns the encoded character or ~0U if
/// the encoding is invalid.
uint32_t validateUTF8CharacterAndAdvance(const char *&Ptr, const char *End);


enum class CommentRetentionMode {
  None,
  AttachToNextToken,
  ReturnAsTokens,
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

class LexerCache final {
public:
  LexerCache();
  ~LexerCache();
};

// TODO: ParsingOptions
class Lexer final : public LexerBase {

  const unsigned BufferID;
  const SrcMgr &sm;
  DiagnosticEngine *de;
  StatsReporter *se;
  LexerState state;

  /// A queue of diagnostics to emit when a token is consumed. We want to queue
  /// them, as the parser may backtrack and re-lex a token.
  std::optional<DiagnosticQueue> DiagQueue;

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

  /// The location at which the comment of the next token starts. \c nullptr if
  /// the next token doesn't have a comment.
  const char *CommentStart;

  /// If this is not \c nullptr, all tokens after this point are treated as eof.
  /// Used to cut off lexing early when we detect that the nesting level is too
  /// deep.
  const char *LexerCutOffPoint = nullptr;

  Lexer(const Lexer &) = delete;
  void operator=(const Lexer &) = delete;

  struct PrincipalLexer {};

  /// The principal constructor used by public constructors below.
  /// Don't use this constructor for other purposes, it does not initialize
  /// everything.
  Lexer(const PrincipalLexer &, unsigned BufferID, const SrcMgr &sm,
        DiagnosticEngine *de, StatsReporter *SE, LexerMode LexMode,
        HashbangMode HashbangAllowed, CommentRetentionMode RetainComments);

  void LexImpl();
  void Initialize(unsigned Offset, unsigned EndOffset);

  /// Retrieve the diagnostic engine for emitting diagnostics for the current
  /// token.
  DiagnosticEngine *getTokenDiags() {
    return DiagQueue ? &DiagQueue->getDiags() : nullptr;
  }

  /// Retrieve the underlying diagnostic engine we emit diagnostics to. Note
  /// this should only be used for diagnostics not concerned with the current
  /// token.
  DiagnosticEngine *getUnderlyingDiags() const {
    return DiagQueue ? &DiagQueue->getUnderlyingDiags() : nullptr;
  }

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
  Lexer(unsigned BufferID, const SrcMgr &sm, DiagnosticEngine *de,
        StatsReporter *se, LexerMode LexMode,
        HashbangMode HashbangAllowed = HashbangMode::Disallowed,
        CommentRetentionMode RetainComments = CommentRetentionMode::None);

  Lexer(unsigned BufferID, const SrcMgr &sm, DiagnosticEngine *de,
        StatsReporter *se);

  /// Create a lexer that scans a subrange of the source buffer.
  Lexer(unsigned BufferID, const SrcMgr &sm, stone::DiagnosticEngine *de,
        StatsReporter *se, LexerMode LexMode, HashbangMode HashbangAllowed,
        CommentRetentionMode RetainComments, unsigned Offset,
        unsigned EndOffset);

  /// Create a sub-lexer that lexes from the same buffer, but scans
  /// a subrange of the buffer.
  ///
  /// \param Parent the parent lexer that scans the whole buffer
  /// \param BeginState start of the subrange
  /// \param EndState end of the subrange
  Lexer(Lexer &Parent, LexerState BeginState, LexerState EndState);

  /// Returns true if this lexer will produce a code completion token.
  bool isCodeCompletion() const { return CodeCompletionPtr != nullptr; }

  /// Whether we are lexing a Swift interface file.
  bool IsStoneInterface() const { return LexMode == LexerMode::StoneInterface; }

  /// Lex a token. If \c TriviaRetentionMode is \c WithTrivia, passed pointers
  /// to trivias are populated.
  void Lex(Token &Result) {
    Result = NextToken;
    // if (DiagQueue){
    //   DiagQueue->emit();
    // }

    if (Result.IsNot(tok::eof)) {
      LexImpl();
    }
  }
  /// Reset the lexer's buffer pointer to \p Offset bytes after the buffer
  /// start.
  void resetToOffset(size_t Offset) {
    assert(BufferStart + Offset <= BufferEnd && "Offset after buffer end");
    CurPtr = BufferStart + Offset;
    LexImpl();
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
  LexerState getStateForBeginningOfToken(const Token &Tok) const {

    // If the token has a comment attached to it, rewind to before the comment,
    // not just the start of the token.  This ensures that we will re-lex and
    // reattach the comment to the token if rewound to this state.
    SrcLoc TokStart = Tok.GetCommentStart();
    if (TokStart.isInvalid()) {
      TokStart = Tok.GetLoc();
    }
    return getStateForBeginningOfTokenLoc(TokStart);
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
    LexImpl();

    // TODO: Don't re-emit diagnostics from readvancing the lexer.
    if (DiagQueue && !enableDiagnostics) {
      /// DiagQueue->clear();
    }
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
      const SrcMgr &SM, SrcLoc Loc,
      CommentRetentionMode CRM = CommentRetentionMode::ReturnAsTokens);

  /// Retrieve the source location that points just past the
  /// end of the token referred to by \c Loc.
  ///
  /// \param SM The source manager in which the given source location
  /// resides.
  ///
  /// \param Loc The source location of the beginning of a token.
  static SrcLoc GetLocForEndOfTokenImpl(const SrcMgr &SM, SrcLoc Loc);

  /// Convert a SrcRange to the equivalent CharSrcRange
  ///
  /// \param SM The source manager in which the given source range
  /// resides.
  ///
  /// \param SR The source range
  static CharSrcRange getCharSrcRangeFromSrcRange(const SrcMgr &SM,
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
  static SrcLoc GetLocForStartOfToken(SrcMgr &SM, unsigned BufferID,
                                      unsigned Offset);

  static SrcLoc GetLocForStartOfToken(SrcMgr &SM, SrcLoc Loc);

  /// Retrieve the start location of the line containing the given location.
  /// the given location.
  static SrcLoc GetLocForStartOfLine(SrcMgr &SM, SrcLoc Loc);

  /// Retrieve the source location for the end of the line containing the
  /// given location, which is the location of the start of the next line.
  static SrcLoc GetLocForEndOfLine(SrcMgr &SM, SrcLoc Loc);

  /// Retrieve the string used to indent the line that contains the given
  /// source location.
  ///
  /// If \c ExtraIndentation is not null, it will be set to an appropriate
  /// additional intendation for adding code in a smaller scope "within" \c Loc.
  static StringRef getIndentationForLine(SrcMgr &SM, SrcLoc Loc,
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
                                       stone::DiagnosticEngine *Diags);

  void getStringLiteralSegments(const Token &Str,
                                SmallVectorImpl<StringSegment> &Segments) {
    return getStringLiteralSegments(Str, Segments, de);
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

  InFlightDiagnostic diagnose(const char *loc, Diagnostic diagnostic);

  template <typename... DiagArgTypes, typename... ArgTypes>
  InFlightDiagnostic diagnose(const char *loc, Diag<DiagArgTypes...> DiagID,
                              ArgTypes &&...Args) {
    return diagnose(loc, Diagnostic(DiagID, std::forward<ArgTypes>(Args)...));
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
  static unsigned lexUnicodeEscape(const char *&CurPtr, Lexer *Diags);

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

public:
  Token GetTokenAtLoc(const SrcMgr &sm, SrcLoc loc) override {
    return getTokenAtLocation(sm, loc);
  }
  SrcLoc GetLocForEndOfToken(const SrcMgr &sm, SrcLoc loc) override {
    return GetLocForEndOfTokenImpl(sm, loc);
  }
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
} // namespace stone

#endif