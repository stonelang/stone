#include "stone/Parse/Parser.h"

using namespace stone;
using namespace stone::syn;

ParsingDeclSpecifier::ParsingDeclSpecifier(Parser &parser)
    : DeclSpecifier(parser.GetAttributeFactory()), parser(parser) {}

PairDelimiterBalancer::PairDelimiterBalancer(Parser &other)
    : parser(other), parenCount(other.parenCount),
      bracketCount(other.bracketCount), braceCount(other.braceCount) {}

PairDelimiterBalancer::~PairDelimiterBalancer() {
  // parser.AngleBrackets.clear(parser);
  parser.parenCount = parenCount;
  parser.bracketCount = bracketCount;
  parser.braceCount = braceCount;
}

//=Scope=//
ParsingScope::ParsingScope(Parser *self, SyntaxScopeKind scopeKind,
                           bool enteredScope, bool beforeCompoundStmt)
    : self(self) {
  if (enteredScope && !beforeCompoundStmt) {
    self->EnterScope(scopeKind);
  } else {
    if (beforeCompoundStmt) {
      // TODO: self->incrementMSManglingNumber();
    }
    this->self = nullptr;
  }
}

// Exit - Exit the scope associated with this object now, rather
// than waiting until the object is destroyed.
void ParsingScope::Exit() {
  if (self) {
    self->ExitScope();
    self = nullptr;
  }
}
ParsingScope::~ParsingScope() { Exit(); }