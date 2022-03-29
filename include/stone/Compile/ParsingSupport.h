#ifndef STONE_COMPILE_PARSINGSUPPORT_H
#define STONE_COMPILE_PARSINGSUPPORT_H

#include "stone/Syntax/DeclSpecifier.h"
#include "llvm/ADT/ArrayRef.h"

namespace stone {
namespace syn {

class Parser;
class ParsingDeclContext {};

/// A class for parsing a DeclSpecifier.
class ParsingDeclSpecifier final : public DeclSpecifier {
  //   ParsingDeclRAII parsingDeclRAII;
  Parser &parser;

public:
  // TODO: There is more to this
  ParsingDeclSpecifier(Parser &parser);

  //   ParsingDeclSpecifier(Parser &P, ParsingDeclRAIIObject *RAII)
  //     : DeclSpecifier(P.getAttrFactory()),
  //       ParsingDeclRAII(P, RAII) {}

  //   const sema::DelayedDiagnosticPool &getDelayedDiagnosticPool() const {
  //     return ParsingDeclRAII.GetDelayedDiagnosticPool();
  //   }

  //   void Complete(Decl *D) {
  //     ParsingDeclRAII.complete(D);
  //   }

  //   void Aort() {
  //     ParsingDeclRAII.abort();
  //   }
};

/// A class for parsing a declarator.
class ParsingDeclarator final : public Declarator {
  // ParsingDeclRAIIObject ParsingRAII;

  // public:
  //   ParsingDeclarator(Parser &P, const ParsingDeclSpecifier &ds,
  //   DeclaratorContext dc)
  //       : Declarator(DS, C), ParsingRAII(P, &DS.getDelayedDiagnosticPool())
  //       {}

  //   const ParsingDeclSpec &GetDeclSpecifier() const {
  //     return static_cast<const ParsingDeclSpec &>(Declarator::getDeclSpec());
  //   }

  //   ParsingDeclSpec &getMutableDeclSpecifier() const {
  //     return const_cast<ParsingDeclSpec &>(getDeclSpec());
  //   }

  //   void Clear() {
  //     //Declarator::Clear();
  //     //ParsingRAII.Reset();
  //   }

  // void Complete(Decl *D) { ParsingRAII.complete(D); }
};
/// RAII object that makes sure paren/bracket/brace count is correct
/// after declaration/statement parsing, even when there's a parsing error.
class PairDelimiterBalancer final {
  Parser &parser;
  unsigned short parenCount, bracketCount, braceCount;

public:
  PairDelimiterBalancer(Parser &other);
  ~PairDelimiterBalancer();
};

class Parser;
class ParsingScope final {
  Parser *self;
  ParsingScope(const ParsingScope &) = delete;
  void operator=(const ParsingScope &) = delete;

public:
  // ParseScope - Construct a new object to manage a scope in the
  // parser Self where the new Scope is created with the flags
  // ScopeFlags, but only when we aren't about to enter a compound statement --
  // may just pass SyntaxScope
  ParsingScope(Parser *self, unsigned scopeFlags, bool enteredScope = true,
               bool beforeCompoundStmt = false);

  ~ParsingScope();

public:
  void Exit();
};

class MultiParsingScope final {
public:
};

} // namespace syn
} // namespace stone
#endif
