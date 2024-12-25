#include "stone/Basic/SrcMgr.h"
#include "stone/Basic/Token.h"
#include "stone/Diag/DiagnosticAllocation.h"
#include "stone/Diag/DiagnosticClient.h"
#include "stone/Diag/DiagnosticText.h"

#include "clang/Basic/CharInfo.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/ConvertUTF.h"
#include "llvm/Support/CrashRecoveryContext.h"
#include "llvm/Support/Unicode.h"
#include "llvm/Support/raw_ostream.h"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <queue>
#include <string>
#include <utility>
#include <vector>

using namespace stone;

enum class TextNodeKind {
  Identifer,
  Percent,
  Select,
  Error,
};

template <typename AlignTy> class TextNodeAllocation {
public:
  /// Disable non-placement new.
  void *operator new(size_t) = delete;
  void *operator new[](size_t) = delete;

  /// Disable non-placement delete.
  void operator delete(void *) = delete;
  void operator delete[](void *) = delete;

  /// Custom version of 'new' that uses the SILModule's BumpPtrAllocator with
  /// precise alignment knowledge.  This is templated on the allocator type so
  /// that this doesn't require including SILModule.h.
  template <typename ContextTy>
  void *operator new(size_t Bytes, const ContextTy &C,
                     size_t Alignment = alignof(AlignTy)) {
    return C.Allocate(Bytes, Alignment);
  }
};
class TextNode : public TextNodeAllocation<TextNode> {
  TextNodeKind Kind;
  llvm::StringRef Text;

protected:
  explicit operator bool() const { return !Text.empty(); }

public:
  TextNode(TextNodeKind Kind, llvm::StringRef Text = StringRef())
      : Kind(Kind), Text(Text) {}

public:
  TextNodeKind GetKind() { return Kind; }
  llvm::StringRef GetText() { return Text; }
};

class IdentiferNode : public TextNode {

public:
  IdentiferNode(llvm::StringRef Text)
      : TextNode(TextNodeKind::Identifer, Text) {}
};

class PercentNode : public TextNode {
  unsigned Index;
  const DiagnosticArgument &Arg;

public:
  PercentNode(unsigned Index, const DiagnosticArgument &Arg)
      : TextNode(TextNodeKind::Percent), Index(Index), Arg(Arg) {}
};

class ModifierNode : public TextNode {
public:
  ModifierNode(TextNodeKind Kind) : TextNode(Kind) {}
};

class SelectNode : public ModifierNode {
public:
  SelectNode() : ModifierNode(TextNodeKind::Select) {}
};

class DirectiveNode : public TextNode {
public:
  DirectiveNode(TextNodeKind Kind) : TextNode(Kind) {}
};

class ErrorNode : public DirectiveNode {
public:
  ErrorNode() : DirectiveNode(TextNodeKind::Error) {}
};

/// Visit node and append to OS
class TextNodeVisitor {
public:
  void Visit();
};

using TextKind = stone::tok;
using TextLexer = stone::diag::DiagnosticTextLexer;

// struct TextLexer {

//   enum class CommentRetentionMode {
//     None,
//     AttachToNextToken,
//     ReturnAsTokens,
//   };

//   enum class TriviaRetentionMode {
//     WithoutTrivia,
//     WithTrivia,
//   };

//   enum class HashbangMode : bool {
//     Disallowed,
//     Allowed,
//   };

//   /// Kinds of conflict marker which the lexer might encounter.
//   enum class ConflictMarkerKind {
//     /// A normal or diff3 conflict marker, initiated by at least 7 "<"s,
//     /// separated by at least 7 "="s or "|"s, and terminated by at least 7
//     ">"s. Normal,
//     /// A Perforce-style conflict marker, initiated by 4 ">"s,
//     /// separated by 4 "="s, and terminated by 4 "<"s.
//     Perforce
//   };

//   class LexerState final {
//     friend class TextLexer;

//   public:
//     LexerState() {}
//     bool IsValid() const { return loc.isValid(); }

//     LexerState Advance(unsigned offset) const {
//       assert(IsValid());
//       return LexerState(loc.getAdvancedLoc(offset));
//     }

//   private:
//     explicit LexerState(SrcLoc loc) : loc(loc) {}
//     SrcLoc loc;
//     llvm::StringRef leadingTrivia;
//     friend class Lexer;
//   };

//   /// Nul character meaning kind.
//   enum class NullCharacterKind {
//     /// String buffer terminator.
//     BufferEnd,
//     /// Embedded nul character.
//     Embedded,
//     /// Code completion marker.
//     CodeCompletion
//   };

//   const unsigned BufferID;

//   const SrcMgr &SM;

//   llvm::raw_ostream &Diag;

//   LexerState state;

//   /// Pointer to the first character of the buffer, even in a lexer that
//   /// scans a subrange of the buffer.
//   const char *BufferStart;

//   /// Pointer to one past the end character of the buffer, even in a lexer
//   /// that scans a subrange of the buffer.  Because the buffer is always
//   /// NUL-terminated, this points to the NUL terminator.
//   const char *BufferEnd;

//   /// Pointer to the artificial EOF that is located before BufferEnd.  Useful
//   /// for lexing subranges of a buffer.
//   const char *ArtificialEOF = nullptr;

//   /// If non-null, points to the '\0' character in the buffer where we should
//   /// produce a code completion token.
//   const char *CodeCompletionPtr = nullptr;

//   /// Points to BufferStart or past the end of UTF-8 BOM sequence if it
//   exists. const char *ContentStart;

//   /// Pointer to the next not consumed character.
//   const char *CurPtr;

//   Token NextToken;

//   /// True if we should skip past a `#!` line at the start of the file.
//   const bool IsHashbangAllowed;

//   const CommentRetentionMode RetainComments;

//   const TriviaRetentionMode TriviaRetention;

//   /// The current leading trivia for the next token.
//   ///
//   /// The StringRef points into the source buffer that is currently being
//   lexed. llvm::StringRef LeadingTrivia;

//   /// The current trailing trivia for the next token.
//   /// The StringRef points into the source buffer that is currently being
//   lexed. llvm::StringRef TrailingTrivia;

//   /// The location at which the comment of the next token starts. \c nullptr
//   if
//   /// the next token doesn't have a comment.
//   const char *CommentStart;

//   /// If this is not \c nullptr, all tokens after this point are treated as
//   eof.
//   /// Used to cut off lexing early when we detect that the nesting level is
//   too
//   /// deep.
//   const char *LexerCutOffPoint = nullptr;

//   //=Lexer options goes here=/
//   bool warnOnEditorPlaceholder = false;
//   static constexpr unsigned extraIndentationSize = 4;

//   TextLexer(bool primary, unsigned BufferID, const SrcMgr &SM,
//             llvm::raw_ostream &Diag, HashbangMode HashbangAllowed,
//             CommentRetentionMode RetainComments,
//             TriviaRetentionMode TriviaRetention)
//       : BufferID(BufferID), SM(SM), Diag(Diag),
//         IsHashbangAllowed(HashbangAllowed == HashbangMode::Allowed),
//         RetainComments(RetainComments), TriviaRetention(TriviaRetention) {}

//   void Init(unsigned Offset, unsigned EndOffset) {
//     assert(Offset <= EndOffset);

//     // Initialize buffer pointers.
//     StringRef contents = SM.extractText(SM.getRangeForBuffer(BufferID));
//     BufferStart = contents.data();
//     BufferEnd = contents.data() + contents.size();
//     assert(*BufferEnd == 0);
//     assert(BufferStart + Offset <= BufferEnd);
//     assert(BufferStart + EndOffset <= BufferEnd);

//     // Check for Unicode BOM at start of file (Only UTF-8 BOM supported now).
//     size_t BOMLength = contents.starts_with("\xEF\xBB\xBF") ? 3 : 0;

//     // Keep information about existance of UTF-8 BOM for transparency source
//     // code editing with libSyntax.
//     ContentStart = BufferStart + BOMLength;

//     // Initialize code completion.
//     if (BufferID == SM.getCodeCompletionBufferID()) {
//       const char *Ptr = BufferStart + SM.getCodeCompletionOffset();
//       if (Ptr >= BufferStart && Ptr <= BufferEnd)
//         CodeCompletionPtr = Ptr;
//     }

//     ArtificialEOF = BufferStart + EndOffset;
//     CurPtr = BufferStart + Offset;

//     assert(NextToken.Is(tok::LAST));

//     Lex();
//     assert((NextToken.IsAtStartOfLine() || CurPtr != BufferStart) &&
//            "The token should be at the beginning of the line, "
//            "or we should be lexing from the middle of the buffer");
//   }

//   TextLexer(unsigned BufferID, const stone::SrcMgr &SM, llvm::raw_ostream
//   &Diag,
//             HashbangMode HashbangAllowed, CommentRetentionMode
//             RetainComments, TriviaRetentionMode TriviaRetention)
//       : TextLexer(true, BufferID, SM, Diag, HashbangAllowed, RetainComments,
//                   TriviaRetention) {

//     unsigned EndOffset = SM.getRangeForBuffer(BufferID).getByteLength();

//     Init(/*Offset=*/0, EndOffset);
//   }

//   TextLexer(unsigned BufferID, const stone::SrcMgr &SM, llvm::raw_ostream
//   &Diag)
//       : TextLexer(BufferID, SM, Diag, HashbangMode::Disallowed,
//                   CommentRetentionMode::None,
//                   TriviaRetentionMode::WithoutTrivia) {}

//   TextLexer(unsigned BufferID, const stone::SrcMgr &SM, llvm::raw_ostream
//   &Diag,
//             HashbangMode HashbangAllowed, CommentRetentionMode
//             RetainComments, TriviaRetentionMode TriviaRetention, unsigned
//             Offset, unsigned EndOffset)
//       : TextLexer(true, BufferID, SM, Diag, HashbangAllowed, RetainComments,
//                   TriviaRetention) {

//     Init(Offset, EndOffset);
//   }

//   TextLexer(TextLexer &Parent, LexerState BeginState, LexerState EndState)
//       : TextLexer(true, Parent.BufferID, Parent.SM, Parent.Diag,
//                   Parent.IsHashbangAllowed ? HashbangMode::Allowed
//                                            : HashbangMode::Disallowed,
//                   Parent.RetainComments, Parent.TriviaRetention) {

//     assert(BufferID == SM.findBufferContainingLoc(BeginState.loc) &&
//            "state for the wrong buffer");
//     assert(BufferID == SM.findBufferContainingLoc(EndState.loc) &&
//            "state for the wrong buffer");

//     unsigned Offset = SM.getLocOffsetInBuffer(BeginState.loc, BufferID);
//     unsigned EndOffset = SM.getLocOffsetInBuffer(EndState.loc, BufferID);

//     Init(Offset, EndOffset);
//   }

//   /// Returns true if this lexer will produce a code completion token.
//   bool IsCodeCompletion() const { return CodeCompletionPtr != nullptr; }

//   /// Lex a token. If \c TriviaRetentionMode is \c WithTrivia, passed
//   pointers
//   /// to trivias are populated.
//   void Lex(Token &Result, llvm::StringRef &LeadingTriviaResult,
//            llvm::StringRef &TrailingTriviaResult) {
//     Result = NextToken;
//     if (TriviaRetention == TriviaRetentionMode::WithTrivia) {
//       LeadingTriviaResult = LeadingTrivia;
//       TrailingTriviaResult = TrailingTrivia;
//     }
//     if (!Result.IsEOF()) {
//       Lex();
//     }
//   }

//   void Lex(Token &Result) {
//     llvm::StringRef LeadingTrivia, TrailingTrivia;
//     Lex(Result, LeadingTrivia, TrailingTrivia);
//   }

//   /// Reset the lexer's buffer pointer to \p Offset bytes after the buffer
//   /// start.
//   void ResetToOffset(size_t Offset) {
//     assert(BufferStart + Offset <= BufferEnd && "Offset after buffer end");
//     CurPtr = BufferStart + Offset;
//     Lex();
//   }

//   Token GetTokenAt(SrcLoc Loc) {
//     assert(BufferID == static_cast<unsigned>(SM.findBufferContainingLoc(Loc))
//     &&
//            "location from the wrong buffer");

//     TextLexer L(BufferID, SM, Diag, HashbangMode::Allowed,
//                 CommentRetentionMode::None,
//                 TriviaRetentionMode::WithoutTrivia);

//     // L.RestoreState(LexerState(Loc));
//     // return L.Peek();
//   }

//   // Cut off lexing at the current position. The next token to be lexed will
//   /// be an EOF token, even if there is still source code to be lexed.
//   /// The current and next token (returned by \c Peek ) are not
//   /// modified. The token after \c NextToken will be the EOF token.
//   void CutOffLexing() {
//     // If we already have a cut off point, don't push it further towards the
//     // back.
//     if (LexerCutOffPoint == nullptr || LexerCutOffPoint >= CurPtr) {
//       LexerCutOffPoint = CurPtr;
//     }
//   }

//   /// If a lexer cut off point has been set returns the offset in the buffer
//   at
//   /// which lexing is being cut off.
//   std::optional<size_t> LexingCutOffOffset() const {
//     if (LexerCutOffPoint) {
//       return LexerCutOffPoint - BufferStart;
//     } else {
//       return std::nullopt;
//     }
//   }

//   bool IsKeepingComments() const {
//     return RetainComments == CommentRetentionMode::ReturnAsTokens;
//   }

//   unsigned GetBufferID() const { return BufferID; }

//   /// Peek - Return the next token to be returned by Lex without
//   /// actually lexing it.
//   const Token &PeekNext() const { return NextToken; }

//   bool IsCurPtrOutOfRange() const {
//     return (CurPtr >= BufferStart && CurPtr <= BufferEnd);
//   }

//   explicit operator bool() const {
//     return (LexerCutOffPoint && CurPtr >= LexerCutOffPoint);
//   }

//   void FormToken(TextKind Kind, const char *TokStart) {
//     assert(CurPtr >= BufferStart && CurPtr <= BufferEnd &&
//            "Current pointer out of range!");
//   }
//   void FormEscapedIdentifierToken(const char *TokStart);
//   void FormStringLiteralToken(const char *TokStart, bool IsMultilineString,
//                               unsigned CustomDelimiterLen);

//   /// Advance to the end of the line.
//   /// If EatNewLine is true, CurPtr will be at end of newline character.
//   /// Otherwise, CurPtr will be at newline character.
//   void SkipToEndOfLine(bool EatNewline);

//   /// Skip to the end of the line of a // comment.
//   void SkipSlashSlashComment(bool EatNewline);

//   /// Skip a #! hashbang line.
//   void SkipHashbang(bool EatNewline);

//   void SkipSlashStarComment();

//   void LexHash();
//   void LexIdentifier();
//   void LexDollarIdent();
//   void LexOperatorIdentifier();
//   void LexHexNumber();
//   void LexNumber();

//   /// Skip over trivia and return characters that were skipped over in a \c
//   /// StringRef. \p AllTriviaStart determines the start of the trivia. In
//   nearly
//   /// all cases, this should be \c CurPtr. If other trivia has already been
//   /// skipped over (like a BOM), this can be used to point to the start of
//   the
//   /// BOM. The returned \c StringRef will always start at \p AllTriviaStart.
//   llvm::StringRef LexTrivia(bool IsForTrailingTrivia,
//                             const char *AllTriviaStart);
//   static unsigned LexUnicodeEscape(const char *&CurPtr, TextLexer *Diags);

//   unsigned LexCharacter(const char *&CurPtr, char StopQuote,
//                         bool EmitDiagnostics, bool IsMultilineString = false,
//                         unsigned CustomDelimiterLen = 0);
//   void LexStringLiteral(unsigned CustomDelimiterLen = 0);
//   void LexEscapedIdentifier();

//   void LexRegexLiteral(const char *TokStart);

//   void TryLexEditorPlaceholder();
//   const char *TindEndOfCurlyQuoteStringLiteral(const char *,
//                                                bool EmitDiagnostics);

//   /// Try to lex conflict markers by checking for the presence of the start
//   and
//   /// end of the marker in diff3 or Perforce style respectively.
//   bool TryLexConflictMarker(bool EatNewline);

//   /// Returns it should be tokenize.
//   bool LexAlien(bool EmitDiagnosticsIfToken);

//   NullCharacterKind GetNullCharacterKind(const char *Ptr) const;

//   /// Emit diagnostics for single-quote string and suggest replacement
//   /// with double-quoted equivalent.
//   ///
//   /// Or, if we're in strawperson mode, we will emit a custom
//   /// error message instead, determined by the Swift library.
//   void DiagnoseSingleQuoteStringLiteral(const char *TokStart,
//                                         const char *TokEnd);

//   /// Is the operator beginning at the given character "left-bound"?
//   static bool IsLeftBound(const char *tokBegin, const char *bufferBegin) {
//     // The first character in the file is not left-bound.
//     if (tokBegin == bufferBegin) {
//       return false;
//     }

//     switch (tokBegin[-1]) {
//     case ' ':
//     case '\r':
//     case '\n':
//     case '\t': // whitespace
//     case '(':
//     case '[':
//     case '{': // opening delimiters
//     case ',':
//     case ';':
//     case ':':  // expression separators
//     case '\0': // whitespace / last char in file
//       return false;

//     case '/':
//       if (tokBegin - 1 != bufferBegin && tokBegin[-2] == '*')
//         return false; // End of a slash-star comment, so whitespace.
//       else
//         return true;

//     case '\xA0':
//       if (tokBegin - 1 != bufferBegin && tokBegin[-2] == '\xC2')
//         return false; // Non-breaking whitespace (U+00A0)
//       else
//         return true;

//     default:
//       return true;
//     }
//   }

//   /// Is the operator ending at the given character (actually one past the
//   end)
//   /// "right-bound"?
//   ///
//   /// The code-completion point is considered right-bound.
//   bool IsRightBound(const char *tokEnd, bool isLeftBound,
//                     const char *codeCompletionPtr) {
//     switch (*tokEnd) {
//     case ' ':
//     case '\r':
//     case '\n':
//     case '\t': // whitespace
//     case ')':
//     case ']':
//     case '}': // closing delimiters
//     case ',':
//     case ';':
//     case ':': // expression separators
//       return false;

//     case '\0':
//       if (tokEnd == codeCompletionPtr) // code-completion
//         return true;
//       return false; // whitespace / last char in file

//     case '.':
//       // Prefer the '^' in "x^.y" to be a postfix op, not binary, but the '^'
//       in
//       // "^.y" to be a prefix op, not binary.
//       return !isLeftBound;

//     case '/':
//       // A following comment counts as whitespace, so this token is not right
//       // bound.
//       if (tokEnd[1] == '/' || tokEnd[1] == '*')
//         return false;
//       else
//         return true;

//     case '\xC2':
//       if (tokEnd[1] == '\xA0')
//         return false; // Non-breaking whitespace (U+00A0)
//       else
//         return true;

//     default:
//       return true;
//     }
//   }

//   void Lex() {
//     assert(IsCurPtrOutOfRange() && "Current pointer is out of range!");

//     const char *LeadingTriviaStart = CurPtr;
//     if (CurPtr == BufferStart) {
//       if (BufferStart < ContentStart) {
//         size_t BOMLen = ContentStart - BufferStart;
//         assert(BOMLen == 3 && "UTF-8 BOM is 3 bytes");
//         CurPtr += BOMLen;
//       }
//       NextToken.SetAtStartOfLine(true);
//     } else {
//       NextToken.SetAtStartOfLine(false);
//     }

//     // Remember the start of the token so we can form the text range.
//     const char *TokStart = CurPtr;

//     // Are we beyound the cut off
//     if (!this) {
//       return FormToken(TextKind::eof, TokStart);
//     }
//   }
// };

class TextParser {

  llvm::raw_ostream &Out;

  llvm::ArrayRef<DiagnosticArgument> Args;

  TextLexer Lexer;

  // Keep track of the args we have processed
  unsigned ParsedArgsCount = 0;

  // This is the previous token pasrsed by the parser.
  Token PrevTok;

  /// This is the current token being considered by the parser.
  Token CurTok;

  /// The location of the previous tok.
  SrcLoc PrevLoc;

  /// Leading trivia for \c Tok.
  /// Always empty if !SF.shouldBuildSyntaxTree().
  llvm::StringRef LeadingTrivia;

  /// Trailing trivia for \c Tok.
  /// Always empty if !SF.shouldBuildSyntaxTree().
  llvm::StringRef TrailingTrivia;

  llvm::SmallVector<TextNode> nodes;

  /// The allocator used to create Diagnostics -- released only when the
  /// DiagnosticEngine is destroyed
  mutable llvm::BumpPtrAllocator allocator;

  bool IsEOF() { return CurTok.GetKind() == tok::eof; }
  bool IsParsing() { return !IsEOF(); }

  void Lex(Token &result) { Lexer.Lex(result); }
  void Lex(Token &result, llvm::StringRef &leading, llvm::StringRef &trailing) {
    Lexer.Lex(result, leading, trailing);
  }

  SrcLoc Consume(tok kind) {
    assert(CurTok.Is(kind) && "Consuming wrong curTok type");
    return Consume();
  }

  /// If the current curTok is the collectorified kind, consume it and
  /// return true.  Otherwise, return false without consuming it.
  bool ConsumeIf(tok kind) {
    if (CurTok.IsNot(kind)) {
      return false;
    }
    Consume(kind);
    return true;
  }
  /// If the current curTok is the collectorified kind, consume it and
  /// return true.  Otherwise, return false without consuming it.
  bool ConsumeIf(tok kind, SrcLoc &consumedLoc) {
    if (CurTok.IsNot(kind)) {
      return false;
    }
    consumedLoc = Consume(kind);
    return true;
  }
  const Token &PeekNext() const { return Lexer.Peek(); }

  SrcLoc Consume() {
    PrevTok = CurTok;
    auto CurLoc = CurTok.GetLoc();
    assert(CurTok.IsNot(tok::eof) && "Lexing past eof!");
    Lex(CurTok, LeadingTrivia, TrailingTrivia);
    PrevLoc = CurLoc;
    return CurLoc;
  }

  std::optional<TextNode *> ParseStringLiteralNode() {
    assert(CurTok.GetKind() == tok::string_literal && "Expecting identifier");
    /// The only string literal we have are of the form "'%N'"
    const char *CurPtr = CurTok.GetText().begin();
    const char *EndPtr = CurTok.GetText().end();
    while (CurPtr != EndPtr) {
      if (*CurPtr == '%') {
        CurPtr++;
        assert(clang::isDigit(*CurPtr) && "Expecting a number after '%' ");
        unsigned IntextIndex = *CurPtr - '0';
        assert(IntextIndex == ParsedArgsCount &&
               "in-text index must math arg-index");
        Consume();
        ++ParsedArgsCount;
        return new (*this) PercentNode(ParsedArgsCount, Args[ParsedArgsCount]);
      }
      CurPtr++;
    }
    return std::nullopt;
  }

  std::optional<TextNode *> ParseIdentifierNode() { return nullptr; }

public:
  TextParser(unsigned BufferID, SrcMgr &SM, llvm::raw_ostream &Out,
             ArrayRef<DiagnosticArgument> Args)
      : Lexer(BufferID, SM, llvm::errs()), Out(Out), Args(Args) {}

  llvm::BumpPtrAllocator &GetAllocator() { return allocator; }
  /// Allocate - Allocate memory from the Driver bump pointer.
  void *Allocate(unsigned long Bytes, unsigned Alignment = 8) const {
    if (Bytes == 0) {
      return nullptr;
    }
    return allocator.Allocate(Bytes, Alignment);
  }

public:
  void Parse(std::vector<TextNode> &results) {

    if (CurTok.IsLast()) {
      // Consume();
    }
    while (IsParsing()) {
      auto resutl = ParseNode();
    }
  }
  std::optional<TextNode *> ParseNode() {
    switch (CurTok.GetKind()) {
    case tok::string_literal: {
      return ParseStringLiteralNode();
    }
    case tok::identifier: {
      return ParseIdentifierNode();
    }
    }
  }
};

class TextFormatter {
public:
  void Format(llvm::ArrayRef<TextNode> nodes) {
    // for (auto slice : slices) {
    // }
    // call vistor
  }
};
