#include "stone/Parse/Parse.h"
#include "stone/LangContext.h"
#include "stone/Parse/Parser.h"
#include "stone/CodeCompletionListener.h"
#include "stone/Syntax/Module.h"
#include "stone/Syntax/Syntax.h"

using namespace stone;
using namespace stone::syn;

void syn::Parse(SyntaxFile &sf, Syntax &syntax, SyntaxListener *listener) {

  Parser parser(sf, syntax, listener);
  llvm::SmallVector<SyntaxResult<Decl>> results;
  parser.ParseTopLevelDecls(results);

  if (parser.HasError()) {
    return;
  }
  for (auto result : results) {
    sf.AddTopLevelDecl(result.Get());
  }
}
