#include "stone/Compile/Parse.h"
#include "stone/Basic/Context.h"
#include "stone/Compile/Parser.h"
#include "stone/Compile/SyntaxListener.h"
#include "stone/Syntax/Module.h"
#include "stone/Syntax/Syntax.h"

using namespace stone;
using namespace stone::syn;

void syn::ParseSyntaxFile(SyntaxFile &sf, Syntax &syntax,
                          SyntaxListener *listener) {

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
