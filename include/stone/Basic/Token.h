#ifndef STONE_SUPPORT_TOKEN_H
#define STONE_SUPPORT_TOKEN_H

#include "stone/Basic/LLVM.h"
#include "stone/Basic/SrcLoc.h"
#include "stone/Basic/TokenKind.h"

#include "llvm/ADT/StringRef.h"

namespace stone {

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

  Token() : Token(tok::LAST, {}, 0) {}

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

  // This is good for now.
  bool IsPointerOperator() { return kind == tok::star; }
  bool IsReferenceOperator() { return kind == tok::amp_prefix; }

  bool IsAnyOperator() const {
    return IsBinaryOperator() || kind == tok::oper_postfix ||
           kind == tok::oper_prefix;
  }
  bool IsNotAnyOperator() const { return !IsAnyOperator(); }

  // bool IsEllipsis() const { return IsAnyOperator() && text == "..."; }
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
#define KEYWORD(X)                                                             \
  case tok::kw_##X:                                                            \
    return true;
#include "stone/Basic/TokenKind.def"
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

  bool IsPunctuation() const {
    switch (kind) {
#define PUNCTUATOR(Name, Str)                                                  \
  case tok::Name:                                                              \
    return true;
#include "stone/Basic/TokenKind.def"
    default:
      return false;
    }
  }

  /// Return true if this token is reserved
  bool IsReservedKeyword() const {
    switch (kind) {
#define KEYWORD_RESERVED(Name, Str)                                            \
  case tok::Name:                                                              \
    return true;
#include "stone/Basic/TokenKind.def"
    default:
      return false;
    }
  }

  /// Return true if this token is obsolete
  bool IsDeprecatedKeyword() const {
    switch (kind) {
#define KEYWORD_DEPRECATED(Name, Str)                                          \
  case tok::Name:                                                              \
    return true;
#include "stone/Basic/TokenKind.def"
    default:
      return false;
    }
  }

  bool IsTypeQualifier() const {
    return IsAny(tok::kw_const, tok::kw_restrict, tok::kw_volatile,
                 tok::kw_stone);
  }
  bool IsStorageSpecifier() const {
    return IsAny(tok::kw_auto, tok::kw_register, tok::kw_static);
  }
  bool IsVisibilitySpecifier() const {
    return IsAny(tok::kw_public, tok::kw_internal, tok::kw_private);
  }

  bool IsBuiltinType() const { return Token::IsBuiltinType(kind); }

  bool IsNominalType() const {
    return IsAny(tok::kw_enum, tok::kw_struct, tok::kw_interface,
                 tok::kw_class);
  }
  bool IsFun() const { return kind == tok::kw_fun; }
  bool IsMutable() const { return kind == tok::kw_mutable; }
  bool IsAlias() const { return kind == tok::kw_alias; }
  bool IsPeriod() const { return kind == tok::period; }
  bool IsDoublePipe() const { return kind == tok::doublepipe; }
  bool IsPipe() const { return kind == tok::pipe; }
  bool IsPipeEqual() const { return kind == tok::pipeequal; }
  bool IsEllipsis() const { return kind == tok::ellipsis; }
  bool IsSemi() const { return kind == tok::semi; }
  bool IsEquality() const { return kind == tok::equal; }
  bool IsDoubleEquality() const { return kind == tok::doubleequal; }
  bool IsPound() const { return kind == tok::pound; }
  bool IsAmp() const { return kind == tok::amp; }
  bool IsArrow() const { return kind == tok::arrow; }
  bool IsBackTick() const { return kind == tok::backtick; }
  bool IsTick() const { return kind == tok::tick; }
  bool IsExcliam() const { return kind == tok::exclaim; }
  bool IsDoubleColon() const { return kind == tok::doublecolon; }
  bool IsTilde() const { return kind == tok::tilde; }
  bool IsImport() const { return kind == tok::kw_import; }
  bool IsAuto() const { return kind == tok::kw_auto; }
  bool IsStruct() const { return kind == tok::kw_struct; }
  bool IsClass() const { return kind == tok::kw_class; }
  bool IsInterface() const { return kind == tok::kw_interface; }
  bool IsStone() const { return kind == tok::kw_stone; }
  bool IsConst() const { return kind == tok::kw_const; }
  bool IsInline() const { return kind == tok::kw_inline; }
  bool IsEnum() const { return kind == tok::kw_enum; }
  bool IsStar() const { return kind == tok::star; }
  bool IsLParen() const { return kind == tok::l_paren; }
  bool IsRParen() const { return kind == tok::r_paren; }
  bool IsLBrace() const { return kind == tok::l_brace; }
  bool IsRBrace() const { return kind == tok::r_brace; }
  bool IsLSquare() const { return kind == tok::l_square; }
  bool IsRSquare() const { return kind == tok::r_square; }
  bool IsLast() const { return kind == tok::LAST; }
  bool IsBool() const { return kind == tok::kw_bool; }
  bool IsFalse() const { return kind == tok::kw_false; }
  bool IsTrue() const { return kind == tok::kw_true; }
  bool IsIntegerLiteral() const { return kind == tok::integer_literal; }
  bool IsStringLiteral() const { return kind == tok::integer_literal; }
  bool IsFloatLiteral() const { return kind == tok::integer_literal; }
  bool IsEOF() const { return kind == tok::eof; }
  bool IsNull() const { return kind == tok::kw_null; }
  bool IsThis() const { return kind == tok::kw_this; }
  bool IsTypeChunk() const { return (IsStar() || IsAmp()); }

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

public:
  static bool IsBuiltinType(tok kind) {
    switch (kind) {
    case tok::kw_void:
    case tok::kw_auto:
    case tok::kw_char:
    case tok::kw_char8:
    case tok::kw_char16:
    case tok::kw_char32:
    case tok::kw_int:
    case tok::kw_int8:
    case tok::kw_int16:
    case tok::kw_int32:
    case tok::kw_int64:
    case tok::kw_uint:
    case tok::kw_uint8:
    case tok::kw_uint16:
    case tok::kw_uint32:
    case tok::kw_uint64:
    case tok::kw_float:
    case tok::kw_float32:
    case tok::kw_float64:
    case tok::kw_complex32:
    case tok::kw_complex64:
    case tok::kw_imaginary32:
    case tok::kw_imaginary64:
      return true;
    default:
      return false;
    }
  }
};
} // namespace stone

namespace llvm {
template <typename T> struct isPodLike;
template <> struct isPodLike<stone::Token> {
  static const bool value = true;
};
} // end namespace llvm

#endif