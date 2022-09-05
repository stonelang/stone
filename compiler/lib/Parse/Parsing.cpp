#include "stone/Parse/Parser.h"

using namespace stone;
using namespace stone::syn;

// ParsingDeclSpecifier::ParsingDeclSpecifier(Parser &parser)
//     : DeclSpecifier(parser.GetAttributeFactory()), parser(parser) {}

// PairDelimiterBalancer::PairDelimiterBalancer(Parser &other) : parser(other) {
//   pairDelimiterCount.parenCount = other.pairDelimiterCount.parenCount;
//   pairDelimiterCount.bracketCount = other.pairDelimiterCount.bracketCount;
//   pairDelimiterCount.braceCount = other.pairDelimiterCount.braceCount;
// }
// PairDelimiterBalancer::~PairDelimiterBalancer() {
//   // parser.AngleBrackets.clear(parser);
//   parser.pairDelimiterCount.parenCount = pairDelimiterCount.parenCount;
//   parser.pairDelimiterCount.bracketCount = pairDelimiterCount.bracketCount;
//   parser.pairDelimiterCount.braceCount = pairDelimiterCount.braceCount;
// }

//=Scope=//
ParsingScope::ParsingScope(Parser *self, SyntaxScopeKind scopeKind,
                           bool enteredScope, bool beforeCompoundStmt)
    : self(self) {

  // if (enteredScope && !beforeCompoundStmt) {
  //   self->EnterScope(scopeKind);
  // } else {
  //   if (beforeCompoundStmt) {
  //     // TODO: self->incrementMSManglingNumber();
  //   }
  //   this->self = nullptr;
  // }
}

// Exit - Exit the scope associated with this object now, rather
// than waiting until the object is destroyed.
void ParsingScope::ExitScope() {
  // if (self) {
  //   self->ExitScope();
  //   self = nullptr;
  // }
}
ParsingScope::~ParsingScope() { ExitScope(); }

ParsingPositionRAII::ParsingPositionRAII(Parser &parser)
    : parser(parser), parsingPos(parser.GetParsingPosition()) {}

ParsingPositionRAII::~ParsingPositionRAII() {
  parser.RestoreParsingPosition(parsingPos);
}

ParsingDeclSpecifier::~ParsingDeclSpecifier() { }

ParsingPrettyStackTrace::ParsingPrettyStackTrace(Parser &parser)
    : parser(parser) {}

void ParsingPrettyStackTrace::print(llvm::raw_ostream &out) const {
  out << "With parser at source location: ";
  parser.GetCurTok().GetLoc().print(out, parser.GetSyntaxContext().GetSrcMgr());
  out << '\n';
}
