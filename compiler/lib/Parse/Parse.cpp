#include "stone/Parse/Parse.h"
#include "stone/CodeCompletionListener.h"
#include "stone/Parse/Parser.h"
#include "stone/Public.h"
#include "stone/Syntax/Module.h"
#include "stone/Syntax/SyntaxContext.h"

using namespace stone;
using namespace stone::syn;

void syn::Parse(SyntaxFile &sf, SyntaxContext &sc, SyntaxListener *listener) {

  Parser parser(sf, sc, listener);
  llvm::SmallVector<SyntaxResult<Decl>> results;
  parser.ParseTopLevelDecls(results);

  if (parser.HasError()) {
    return;
  }
  for (auto result : results) {
    if (result.IsNull()) {
      return;
    }
    sf.AddTopLevelDecl(result.Get());
  }
}
