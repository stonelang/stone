#ifndef STONE_PARSE_SYNTAXPARSING_H
#define STONE_PARSE_SYNTAXPARSING_H

#include "stone/Basic/OptionSet.h"
#include "stone/Parse/SyntaxLexing.h"
#include "stone/Syntax/DeclSpecifier.h"
#include "stone/Syntax/SyntaxScope.h"

#include "llvm/ADT/ArrayRef.h"

namespace stone {
namespace syn {

class Parser;
class AttributeFactory;

/// To assist debugging parser crashes, tell us the location of the
/// current curTok.
class SyntaxParsingPrettyStackTrace final : public llvm::PrettyStackTraceEntry {
  Parser &parser;

public:
  explicit SyntaxParsingPrettyStackTrace(Parser &parser);
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

using SyntaxScopeCache = llvm::SmallVector<SyntaxScope *, 16>;
class SyntaxParsingScope final {
  Parser *self;
  SyntaxScopeCache scopeCache;

  bool enteredScope;
  bool beforeCompoundStmt;

  SyntaxParsingScope(const SyntaxParsingScope &) = delete;
  void operator=(const SyntaxParsingScope &) = delete;

public:
  // ParseScope - Construct a new object to manage a scope in the
  // parser Self where the new Scope is created with the flags
  // ScopeFlags, but only when we aren't about to enter a compound statement --
  // may just pass SyntaxScope
  SyntaxParsingScope(Parser *self, SyntaxScopeKind scopeKind,
                     bool enteredScope = true, bool beforeCompoundStmt = false);

  ~SyntaxParsingScope();

public:
  /// EnterScope - start a new scope.
  void EnterScope(SyntaxScopeKind scopeKind);

  /// ExitScope - pop a scope off the scope stack.
  void ExitScope();

  SyntaxScope *GetCurScope() const;
  // size_t GetSyntaxScopeCacheSize() {
  //   return syntaxScopeCache.size();
  // }
};

// class MultiSyntaxParsingScope final {
// public:
// };

// /// The kind of template we are parsing.
// enum class SyntaxParsingTemplateKind : uint8_t {
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
// struct SyntaxParsingTemplate {

//   SyntaxParsingTemplateKind kind;

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

//   SyntaxParsingTemplate()
//       : kind(SyntaxParsingTemplateKind::None),
//       templateParameterLists(nullptr) {
//   }
//   SyntaxParsingTemplate(TemplateParameterLists *templateParameterLists,
//                         bool isSpecialization,
//                         bool lastParameterListWasEmpty = false)
//       : kind(isSpecialization
//                  ? SyntaxParsingTemplateKind::ExplicitSpecialization
//                  : SyntaxParsingTemplateKind::Template),
//         templateParameterLists(templateParameterLists),
//         lastParameterListWasEmpty(lastParameterListWasEmpty) {}

//   explicit SyntaxParsingTemplate(SrcLoc externLoc, SrcLoc templateLoc)
//       : kind(SyntaxParsingTemplateKind::ExplicitInstantiation),
//         templateParameterLists(nullptr), externLoc(externLoc),
//         templateLoc(templateLoc), lastParameterListWasEmpty(false) {}

//   SrcRange GetSrcRange() const;
// };

struct DeclSyntaxParsingFlags {
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
using DeclSyntaxParsingOptions = stone::OptionSet<DeclSyntaxParsingFlags::ID>;

enum class SyntaxParsingNotification : UInt8 {
  None,
  DeclCreated,
  StmtCreated,
  ExprCreated,
  TokenConsumed,
};

class SyntaxParsingPosition final {
  friend class Parser;
  SyntaxLexingState lexingState;
  /// TODO: prevTokLoc
  SrcLoc prevLoc;

  SyntaxParsingPosition(SyntaxLexingState lexingState, SrcLoc prevLoc)
      : lexingState(lexingState), prevLoc(prevLoc) {}

public:
  SyntaxParsingPosition() = default;
  SyntaxParsingPosition &operator=(const SyntaxParsingPosition &) = default;

  bool isValid() const { return lexingState.IsValid(); }
};

/// RAII object that, when it is destructed, restores the parser and lexer to
/// their positions at the time the object was constructed.
struct SyntaxParsingPositionRAII {
private:
  Parser &parser;
  SyntaxParsingPosition parsingPos;

public:
  SyntaxParsingPositionRAII(Parser &parser);
  ~SyntaxParsingPositionRAII();
};

// class SyntaxParsingContext {
// public:
// };

// class SyntaxParsingCache {
// public:
// };

class SyntaxParsingDeclSpecifier final : public DeclSpecifier {

public:
  SyntaxParsingDeclSpecifier(AttributeFactory &attributeFactory)
      : DeclSpecifier(attributeFactory) {}
};

class SyntaxParsingDeclarator final : public Declarator {
  Parser &parser;

public:
  SyntaxParsingDeclarator(Parser &parser,
                          const SyntaxParsingDeclSpecifier &specifier,
                          DeclaratorScopeKind scopeKind)
      : Declarator(specifier, scopeKind), parser(parser) {}
};

class SyntaxParsing {

protected:
  Parser &parser;
  // SyntaxParsingScope syntaxParsingScope;
  // SyntaxParsingCache cache;
  // SyntaxParsingPosition *curPosition;

public:
  SyntaxParsing(Parser &parser) : parser(parser) {}
};

enum class DeclSyntaxParsingStatus : UInt8 { None = 0, Parsing, Error, Done };

class DeclSyntaxParsing final : public SyntaxParsing {
  SyntaxParsingDeclSpecifier *syntaxParsingDeclSpecifier;

public:
  AccessLevel level;
  DeclSyntaxParsingStatus status;
  DeclSyntaxParsingOptions flags;

public:
  DeclSyntaxParsing(Parser &parser) : SyntaxParsing(parser) {}

public:
  TypeSpecifierContext &GetTypeSpecifierContext();
};

} // namespace syn
} // namespace stone
#endif
