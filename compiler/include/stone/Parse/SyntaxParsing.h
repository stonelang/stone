#ifndef STONE_PARSE_PARSINGSUPPORT_H
#define STONE_PARSE_PARSINGSUPPORT_H

#include "stone/Basic/OptionSet.h"
#include "stone/Syntax/DeclSpecifier.h"

#include "llvm/ADT/ArrayRef.h"

namespace stone {
namespace syn {

class Parser;
enum class SyntaxScopeKind : uint8_t;

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
    DisallowInit = 1 << 3,
    AllowDestructor = 1 << 4,
    AllowEnumElement = 1 << 5,
    InInterface = 1 << 6,
    In = 1 << 7,
    InExtension = 1 << 8,
    InStruct = 1 << 9,
    InEnum = 1 << 10,
  };
};
/// Options that control the parsing of declarations.
using DeclSyntaxParsingOpts = stone::OptionSet<DeclSyntaxParsingFlags::ID>;

class SyntaxParsing final {

  // SyntaxParsingScope syntaxParsingScope;

public:
  SyntaxParsing();
};

} // namespace syn
} // namespace stone
#endif
