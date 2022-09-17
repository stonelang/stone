#ifndef STONE_PARSE_PARSINGSUPPORT_H
#define STONE_PARSE_PARSINGSUPPORT_H

#include "stone/Basic/OptionSet.h"
#include "stone/Basic/STDTypeAlias.h"
#include "stone/Parse/Lexing.h"
#include "stone/Syntax/DeclSpecifier.h"
#include "stone/Syntax/Scope.h"
#include "stone/Syntax/SyntaxResult.h"

#include "llvm/ADT/ArrayRef.h"

namespace stone {
namespace syn {

class Parser;
class AttributeFactory;

/// To assist debugging parser crashes, tell us the location of the
/// current curTok.
class ParsingPrettyStackTrace final : public llvm::PrettyStackTraceEntry {
  Parser &parser;

public:
  explicit ParsingPrettyStackTrace(Parser &parser);
  void print(llvm::raw_ostream &out) const override;
};

enum class PairBalancerKind : UInt8 {
  None = 0,
  Bracket,
  Paren,
  Brace,
};
class PairBalancer {

  PairBalancerKind kind;

protected:
  UInt16 count;

public:
  PairBalancer(PairBalancerKind kind, UInt16 count)
      : kind(kind), count(count) {}

public:
  PairBalancerKind GetKind() { return kind; }
  bool IsEven() { return ((count % 2) == 0); }
};
// class ParenBalancer final : public PairBalancer {
//   Parser &parser;
// public:
//   ParenBalancer(Parser &parser)
//       : PairBalancer(PairBalancerKind::Paren, parser.ParenCount),
//         parser(parser) {}

//   ~ParenBalancer() { parser.ParenCount = count; }
// };

using ScopeCache = llvm::SmallVector<Scope *, 16>;
class ParsingScope final {
  Parser &self;
  llvm::StringRef description;

  ParsingScope(const ParsingScope &) = delete;
  void operator=(const ParsingScope &) = delete;

public:
  // ParseScope - Construct a new object to manage a scope in the
  // parser Self where the new Scope is created with the flags
  // ScopeFlags, but only when we aren't about to enter a compound statement --
  // may just pass Scope
  ParsingScope(Parser &self, ScopeKind scopeKind, llvm::StringRef description);
  ~ParsingScope();

private:
  /// EnterScope - start a new scope.
  void EnterScope(ScopeKind scopeKind);

  /// ExitScope - pop a scope off the scope stack.
  void ExitScope();

  llvm::StringRef GetDescription() { return description; }
};

enum class ParsingNotification : UInt8 {
  None,
  DeclCreated,
  StmtCreated,
  ExprCreated,
  TokenConsumed,
};

class ParsingPosition final {
  friend class Parser;
  LexingState lexingState;
  /// TODO: prevTokLoc
  SrcLoc prevLoc;

  ParsingPosition(LexingState lexingState, SrcLoc prevLoc)
      : lexingState(lexingState), prevLoc(prevLoc) {}

public:
  ParsingPosition() = default;
  ParsingPosition &operator=(const ParsingPosition &) = default;

  bool isValid() const { return lexingState.IsValid(); }
};

/// RAII object that, when it is destructed, restores the parser and lexer to
/// their positions at the time the object was constructed.
struct ParsingPositionRAII {
private:
  Parser &parser;
  ParsingPosition parsingPos;

public:
  ParsingPositionRAII(Parser &parser);
  ~ParsingPositionRAII();
};

struct ParsingDeclFlags final {

  ParsingDeclFlags() = delete;

  /// Flags that control the parsing of declarations.
  enum ID {
    Default = 0,
    AllowTopLevel = 1 << 1,
    HasContainerType = 1 << 2,
    AllowDestructor = 1 << 3,
    AllowEnumElement = 1 << 4,
    InInterface = 1 << 5,
    InStruct = 1 << 6,
    InEnum = 1 << 7,
  };
};
/// Options that control the parsing of declarations.
using ParsingDeclOptions = stone::OptionSet<ParsingDeclFlags::ID>;
using ParsingDeclCallback = llvm::function_ref<void(Decl *)>;

class ParsingDeclCollector final : public DeclSpecifier {
  Parser &parser;

public:
  ParsingDeclOptions flags;

public:
  ParsingDeclCollector(Parser &parser, AttributeFactory &attributeFactory)
      : parser(parser), DeclSpecifier(attributeFactory) {}

  ~ParsingDeclCollector();

public:
  Parser &GetParser() { return parser; }
  SyntaxStatus Collect();
  SyntaxStatus CollectSpecifier();
  SyntaxStatus CollectUntil(tok kind);

public:
  void Verify();
  void Apply();
};

class ParsingDeclaratorCollector final : public DeclaratorCollector {
public:
  ParsingDeclaratorCollector(const ParsingDeclCollector &collector,
                             DeclaratorScopeKind declaratorScopeKind)
      : DeclaratorCollector(collector, declaratorScopeKind) {}

public:
  const ParsingDeclCollector &GetParsingDeclCollector() const {
    return static_cast<const ParsingDeclCollector &>(DeclaratorCollector::GetDeclSpecifier());
  }
};

enum class ParsingContextKind : UInt8 {
  None = 0,
  Decl,
  Stmt,
  Expr,
  Type,
};
enum class ParsingContextStatus : UInt8 { None = 0, Parsing, Error, Done };

constexpr size_t ParsingAlignInBits = 3;
class alignas(1 << ParsingAlignInBits) ParsingContext final {
  ParsingContextKind kind;
  ParsingContext *holder = nullptr;

  // ParsingContextStatus status;

public:
  ParsingContext(ParsingContextKind kind, ParsingContext *holder = nullptr)
      : kind(kind) {}
  ParsingContextKind GetKind() { return kind; }

public:
  // bool IsParsing() { return status == ParsingContextStatus::Parsing; }
  // bool IsError() { return status == ParsingContextStatus::Error; }
  // bool IsDone() { return status == ParsingContextStatus::Done; }

  // void SetError() { status = ParsingContextStatus::Error; }
  // void SetParsing() { status = ParsingContextStatus::Parsing; }
  // void SetDone() { status = ParsingContextStatus::Done; }
};

class ParsingToken final {
  Parser &parser;

public:
  ParsingToken(Parser &parser) : parser(parser) {}

public:
  bool IsPeriod();
  bool IsDoublePipe();
  bool IsPipe();
  bool IsPipeEqual();
  bool IsEllipsis();
  bool IsSemi();
  bool IsEquality();
  bool IsDoubleEquality();
  bool IsPound();
  bool IsAmp();
  bool IsArrow();
  bool IsBackTick();
  bool IsExcliam();
  bool IsDoubleColon();
  bool IsTilde();
  bool IsFun();
  bool IsStruct();
  bool IsInterface();
  bool IsPure();
  bool IsInline();
  bool IsEnum();
  bool IsStar();
  bool IsQualifier();
  bool IsAccessLevel();

public:
  bool IsIf();
  bool IsElse();
  bool IsWhile();
  bool IsRightParen();
  bool IsLeftParen();
  bool IsIntegerLiteral();
  bool IsFloatingLiteral();
  bool IsStringLiteral();
  bool IsImaginaryLiteral();
  bool IsRegexLiteral();
};

enum class ParsingDeclAction {
  None = 0,
  ParseAccessLevel,
  ParseAuto,
  ParseFun,
  ParseStruct,
  ParseEnum,
  ParseInterface,
  ParseBasicType,
  ParseTypeQualifier,
  ParsePointer,
  ParseIdentifier,
  ParseImport,
};

} // namespace syn
} // namespace stone
#endif
