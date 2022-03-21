#include "stone/Compile/Parse.h"

#include "stone/Compile/Parser.h"
#include "stone/Compile/SyntaxListener.h"
#include "stone/Core/Context.h"
#include "stone/Syntax/Module.h"
#include "stone/Syntax/Syntax.h"

using namespace stone;
using namespace stone::syn;

void syn::ParseSyntaxFile(SyntaxFile &sf, Syntax &syntax,
                          SyntaxListener *listener) {
  Parser parser(sf, syntax, listener);

  SyntaxResult<Decl *> result;
  while (!parser.IsDone()) {
    parser.ParseTopLevelDecl(result);
    // Check for errors from diag and if there are then exit.
    if (parser.HasError()) {
      if (listener) {
        listener->OnError();
      }
      break;
    }
    if (listener) {
      listener->OnDecl(result.Get(), true);
    }
  }
  if (listener) {
    listener->OnDone();
  }
}
