#ifndef STONE_PARSE_PARSINGSUPPORT_H
#define STONE_PARSE_PARSINGSUPPORT_H

#include "stone/Basic/OptionSet.h"
#include "stone/Parse/Lexing.h"
#include "stone/Syntax/DeclSpecifier.h"
#include "stone/Syntax/Scope.h"

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

// class ParsingDeclContext {
// public:
// };

// /// A class for parsing a DeclSpecifier.
// class ParsingDeclSpecifier final : public DeclSpecifier {
//   //   ParsingDeclRAII parsingDeclRAII;
//   Parser &parser;

// public:
//   // TODO: There is more to this
//   ParsingDeclSpecifier(Parser &parser);

//   //   ParsingDeclSpecifier(Parser &P, ParsingDeclRAIIObject *RAII)
//   //     : DeclSpecifier(P.getAttrFactory()),
//   //       ParsingDeclRAII(P, RAII) {}

//   //   const sema::DelayedDiagnosticPool &getDelayedDiagnosticPool() const {
//   //     return ParsingDeclRAII.GetDelayedDiagnosticPool();
//   //   }

//   //   void Complete(Decl *D) {
//   //     ParsingDeclRAII.complete(D);
//   //   }

//   //   void Abort() {
//   //     ParsingDeclRAII.abort();
//   //   }
// };

// /// A class for parsing a declarator.
// class ParsingDeclarator final : public Declarator {
//   // ParsingDeclRAIIObject ParsingRAII;

//   // public:
//   //   ParsingDeclarator(Parser &P, const ParsingDeclSpecifier &ds,
//   //   DeclaratorContext dc)
//   //       : Declarator(DS, C), ParsingRAII(P,
//   &DS.getDelayedDiagnosticPool())
//   //       {}

//   //   const ParsingDeclSpec &GetDeclSpecifier() const {
//   //     return static_cast<const ParsingDeclSpec
//   &>(Declarator::getDeclSpec());
//   //   }

//   //   ParsingDeclSpec &getMutableDeclSpecifier() const {
//   //     return const_cast<ParsingDeclSpec &>(getDeclSpec());
//   //   }

//   //   void Clear() {
//   //     //Declarator::Clear();
//   //     //ParsingRAII.Reset();
//   //   }

//   // void Complete(Decl *D) { ParsingRAII.complete(D); }
// };

// class PairDelimiterCount final {
// public:
//   unsigned short parenCount;
//   unsigned short bracketCount;
//   unsigned short braceCount;

// public:
//   PairDelimiterCount() : parenCount(0), bracketCount(0), braceCount(0) {}

//   ~PairDelimiterCount() {
//     parenCount = 0;
//     bracketCount = 0;
//     braceCount = 0;
//   }
// };

// /// RAII object that makes sure paren/bracket/brace count is correct
// /// after declaration/statement parsing, even when there's a parsing error.
// class PairDelimiterBalancer final {
//   Parser &parser;

// public:
//   PairDelimiterCount pairDelimiterCount;

// public:
//   PairDelimiterBalancer(Parser &other);
//   ~PairDelimiterBalancer();
// };

using ScopeCache = llvm::SmallVector<Scope *, 16>;
class ScopeContext final {
  Parser &self;
  llvm::StringRef description;

  ScopeContext(const ScopeContext &) = delete;
  void operator=(const ScopeContext &) = delete;

public:
  // ParseScope - Construct a new object to manage a scope in the
  // parser Self where the new Scope is created with the flags
  // ScopeFlags, but only when we aren't about to enter a compound statement --
  // may just pass Scope
  ScopeContext(Parser &self, ScopeKind scopeKind, llvm::StringRef description);
  ~ScopeContext();

private:
  /// EnterScope - start a new scope.
  void EnterScope(ScopeKind scopeKind);

  /// ExitScope - pop a scope off the scope stack.
  void ExitScope();

  llvm::StringRef GetDescription() { return description; }
};

// class MultiParsingScope final {
// public:
// };

// /// The kind of template we are parsing.
// enum class ParsingTemplateKind : uint8_t {
//   /// We are not parsing a template at all.
//   None = 0,
//   /// We are parsing a template declaration.
//   Template,
//   /// We are parsing an explicit specialization.
//   ExplicitSpecialization,
//   /// We are parsing an explicit instantiation.
//   ExplicitInstantiation
// };
// /// Contains information about any template-specific
// /// information that has been parsed prior to parsing declaration
// /// specifiers.
// struct ParsingTemplate {

//   ParsingTemplateKind kind;

//   /// The template parameter lists, for template declarations
//   /// and explicit specializations.
//   TemplateParameterLists *templateParameterLists;

//   /// The location of the 'extern' keyword, if any, for an explicit
//   /// instantiation
//   SrcLoc externLoc;

//   /// The location of the 'template' keyword, for an explicit
//   /// instantiation.
//   SrcLoc templateLoc;

//   /// Whether the last template parameter list was empty.
//   bool lastParameterListWasEmpty;

//   ParsingTemplate()
//       : kind(ParsingTemplateKind::None),
//       templateParameterLists(nullptr) {
//   }
//   ParsingTemplate(TemplateParameterLists *templateParameterLists,
//                         bool isSpecialization,
//                         bool lastParameterListWasEmpty = false)
//       : kind(isSpecialization
//                  ? ParsingTemplateKind::ExplicitSpecialization
//                  : ParsingTemplateKind::Template),
//         templateParameterLists(templateParameterLists),
//         lastParameterListWasEmpty(lastParameterListWasEmpty) {}

//   explicit ParsingTemplate(SrcLoc externLoc, SrcLoc templateLoc)
//       : kind(ParsingTemplateKind::ExplicitInstantiation),
//         templateParameterLists(nullptr), externLoc(externLoc),
//         templateLoc(templateLoc), lastParameterListWasEmpty(false) {}

//   SrcRange GetSrcRange() const;
// };

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

class ParsingDeclSpecifier final : public DeclSpecifier {
  Parser &parser;

public:
  ParsingDeclOptions flags;
  bool DeclCreated = false;

public:
  ParsingDeclSpecifier(Parser &parser, AttributeFactory &attributeFactory)
      : parser(parser), DeclSpecifier(attributeFactory) {}

  ~ParsingDeclSpecifier();

public:
  Parser &GetParser() { return parser; }
};

class ParsingDeclarator final : public Declarator {
public:
  ParsingDeclarator(const ParsingDeclSpecifier &specifier,
                    DeclaratorContextKind contextKind)
      : Declarator(specifier, contextKind) {}

public:
  const ParsingDeclSpecifier &GetParsingDeclSpecifier() {
    return static_cast<const ParsingDeclSpecifier &>(
        Declarator::GetDeclSpecifier());
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
  // ParsingContextStatus status;

public:
  ParsingContext(ParsingContextKind kind) : kind(kind) {}
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
} // namespace syn
} // namespace stone
#endif
