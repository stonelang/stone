#ifndef STONE_CORE_TOKEN_H
#define STONE_CORE_TOKEN_H

#include "stone/Core/LLVM.h"
#include "stone/Core/SrcLoc.h"
#include "stone/Core/TokenKind.h"
#include "llvm/ADT/StringRef.h"

namespace stone {

namespace syn {
class Token final {
  /// The token kind
  tok kind;

  /// Whether this token is the first token on the line.
  unsigned atStartOfLine : 1;

  /// Whether this token is an escaped `identifier` token.
  unsigned escapedIdentifier : 1;

  /// Modifiers for string literals
  unsigned multilineString : 1;

  /// Length of custom delimiter of "raw" string literals
  unsigned customDelimiterLen : 8;

  // Padding bits == 32 - 11;

  /// The length of the comment that precedes the token.
  unsigned commentLength;

  /// text - The actual string covered by the token in the source buffer.
  llvm::StringRef text;

  llvm::StringRef TrimComment() const {
    assert(HasComment() && "Has no comment to trim.");
    StringRef rawStr(text.begin() - commentLength, commentLength);
    return rawStr.trim();
  }

public:
  Token(tok kind, StringRef text, unsigned commentLength = 0)
      : kind(kind), atStartOfLine(false), escapedIdentifier(false),
        multilineString(false), customDelimiterLen(0),
        commentLength(commentLength), text(text) {}

  Token() : Token(tok::MAX, {}, 0) {}

  tok GetKind() const { return kind; }
  void SetKind(tok k) { kind = k; }

  const char *GetName();
  void ClearCommentLength() { commentLength = 0; }

  /// is/isNot - Predicates to check if this token is a specific kind, as in
  /// "if (Tok.is(tok::l_brace)) {...}".
  bool Is(tok k) const { return kind == k; }
  bool IsNot(tok k) const { return kind != k; }

  // Predicates to check to see if the token is any of a list of tokens.

  bool IsAny(tok K1) const { return Is(K1); }
  template <typename... T> bool IsAny(tok K1, tok K2, T... K) const {
    if (Is(K1))
      return true;
    return IsAny(K2, K...);
  }

  // Predicates to check to see if the token is not the same as any of a list.
  template <typename... T> bool IsNot(tok K1, T... K) const {
    return !IsAny(K1, K...);
  }

  bool IsBinaryOperator() const {
    return kind == tok::oper_binary_spaced || kind == tok::oper_binary_unspaced;
  }

  bool IsAnyOperator() const {
    return IsBinaryOperator() || kind == tok::oper_postfix ||
           kind == tok::oper_prefix;
  }
  bool IsNotAnyOperator() const { return !IsAnyOperator(); }

  bool IsEllipsis() const { return IsAnyOperator() && text == "..."; }
  bool IsNotEllipsis() const { return !IsEllipsis(); }

  /// Determine whether this token occurred at the start of a line.
  bool IsAtStartOfLine() const { return atStartOfLine; }

  /// Set whether this token occurred at the start of a line.
  void SetAtStartOfLine(bool value) { atStartOfLine = value; }

  /// True if this token is an escaped identifier token.
  bool IsEscapedIdentifier() const { return escapedIdentifier; }
  /// Set whether this token is an escaped identifier token.
  void SetEscapedIdentifier(bool value) {
    assert((!value || kind == tok::identifier) &&
           "only identifiers can be escaped identifiers");
    escapedIdentifier = value;
  }

  /// True if the token is an identifier or '_'.
  bool IsIdentifierOrUnderscore() const {
    return IsAny(tok::identifier, tok::kw__);
  }

  /// True if the token is an l_paren token that does not start a new line.
  bool IsFollowingLParen() const {
    return !IsAtStartOfLine() && kind == tok::l_paren;
  }

  /// True if the token is an l_square token that does not start a new line.
  bool IsFollowingLSquare() const {
    return !IsAtStartOfLine() && kind == tok::l_square;
  }

  /// True if the token is any keyword.
  bool IsKeyword() const {
    switch (kind) {
#define KEYWORD(X, S)                                                          \
  case tok::kw_##X:                                                            \
    return true;
#include "stone/Core/TokenKind.def"
    default:
      return false;
    }
  }

  /// True if the token is any literal.
  bool IsLiteral() const {
    switch (kind) {
    case tok::integer_literal:
    case tok::floating_literal:
    case tok::string_literal:
      return true;
    default:
      return false;
    }
  }
  bool IsAlien() const { return kind == tok::alien; }

  bool IsPunctuation() const {
    switch (kind) {
#define PUNCTUATOR(Name, Str)                                                  \
  case tok::Name:                                                              \
    return true;
#include "stone/Core/TokenKind.def"
    default:
      return false;
    }
  }

  /// True if the string literal token is multiline.
  bool IsMultilineString() const { return multilineString; }

  /// Count of extending escaping '#'.
  unsigned GetCustomDelimiterLen() const { return customDelimiterLen; }

  /// Set characteristics of string literal token.
  void setStringLiteral(bool isMultilineString, unsigned customDelimiterLen) {
    assert(kind == tok::string_literal);
    this->multilineString = isMultilineString;
    this->customDelimiterLen = customDelimiterLen;
  }
  unsigned GetLength() const { return text.size(); }

  bool HasComment() const { return commentLength != 0; }

  /// GetLoc - Return a source location identifier for the specified
  /// offset in the current file.
  SrcLoc GetLoc() const {
    return SrcLoc(llvm::SMLoc::getFromPointer(text.begin()));
  }

  CharSrcRange GetRange() const { return CharSrcRange(GetLoc(), GetLength()); }

  /*
  CharSourceRange GetCommentRange() const {
    if (CommentLength == 0)
      return
  CharSourceRange(SourceLoc(llvm::SMLoc::getFromPointer(text.begin())), 0);
    auto TrimedComment = trimComment();
    return CharSourceRange(
      SourceLoc(llvm::SMLoc::getFromPointer(TrimedComment.begin())),
      TrimedComment.size());
  }
  SourceLoc GetCommentStart() const {
    if (CommentLength == 0) return SourceLoc();
    return SourceLoc(llvm::SMLoc::getFromPointer(trimComment().begin()));
  }
*/


 SrcLoc GetCommentStart() const {
    if (commentLength == 0)
      return SrcLoc();
    return SrcLoc(llvm::SMLoc::getFromPointer(TrimComment().begin()));
  }



  StringRef GetText() const { return text; }
  void SetText(StringRef T) { text = T; }

  StringRef GetTextWithNoBackticks() const {
    if (escapedIdentifier) {
      // Strip off the backticks on either side.
      assert(text.front() == '`' && text.back() == '`');
      return text.slice(1, text.size() - 1);
    }
    return text;
  }
  /// Set the token to the specified kind and source range.
  void SetToken(tok K, StringRef T, unsigned commentLength = 0) {
    kind = K;
    text = T;
    this->commentLength = commentLength;
    escapedIdentifier = false;
    this->multilineString = false;
    this->customDelimiterLen = 0;
    assert(this->customDelimiterLen == customDelimiterLen &&
           "custom string delimiter length > 255");
  }
};
} // namespace syn
} // namespace stone

namespace llvm {
template <typename T> struct isPodLike;
template <> struct isPodLike<stone::syn::Token> {
  static const bool value = true;
};
} // end namespace llvm

#endif
