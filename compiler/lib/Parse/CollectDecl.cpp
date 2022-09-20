#include "stone/Basic/Defer.h"
#include "stone/Diag/SyntaxDiagnostic.h"
#include "stone/Parse/Parser.h"
#include "stone/Parse/Parsing.h"
#include "stone/Syntax/Stmt.h"
#include "stone/Syntax/SyntaxContext.h"
#include "stone/Syntax/SyntaxFactory.h"
#include "stone/Syntax/SyntaxNode.h"

using namespace stone;
using namespace stone::syn;

SyntaxStatus Parser::CollectDeclSpecifier(ParsingDeclCollector &collector) {}

SyntaxStatus Parser::CollectUsingSpecifier(ParsingDeclCollector &collector) {
  switch (GetCurTok().GetKind()) {
  case tok::kw_using:
    collector.GetUsingDeclarationCollector().AddUsing(ConsumeToken());
    break;
  default:
    return syn::MakeSyntaxCodeCompletionStatus();
  }
  return syn::MakeSyntaxSuccess();
}

SyntaxStatus Parser::CollectAccessLevel(ParsingDeclCollector &collector) {
  //
}

SyntaxStatus Parser::CollectTypeQualifier(ParsingDeclCollector &collector) {
  //
}

SyntaxStatus Parser::CollectTypePatterns(ParsingDeclCollector &collector) {
  //
}
SyntaxStatus
Parser::CollectBasicTypeSpecifier(ParsingDeclCollector &collector) {
  //
}
SyntaxStatus
Parser::CollectNominalTypeSpecifier(ParsingDeclCollector &collector) {
  //
}
SyntaxStatus Parser::CollectStorageSpecifier(ParsingDeclCollector &collector) {
  //
}
SyntaxStatus Parser::CollectFunctionSpecifier(ParsingDeclCollector &collector) {
  switch (GetCurTok().GetKind()) {
  case tok::kw_fun:
    collector.GetFunctionSpecifierCollector().AddFun(ConsumeToken());
    break;
  case tok::kw_inline:
    collector.GetFunctionSpecifierCollector().AddInline(ConsumeToken());
    break;
  default:
    return syn::MakeSyntaxCodeCompletionStatus();
  }
  return syn::MakeSyntaxSuccess();
}
