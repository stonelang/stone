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
  syn::DeclGroupPtrTy topLevelDecl;

  while (!parser.IsDone()) {
    // Check for errors from diag and if there are then exit.
    if (parser.HasError()) {
      if (listener) {
        listener->OnError();
      }
      break;
    }
    // Go through all of the top level decls in the file one at a time
    // As you process a decl, it will be added to the SyntaxFile
    if (parser.ParseTopLevelDecl(topLevelDecl)) {
      // Notifify that a top decl has been parsed.
      if (listener) {
        listener->OnDecl(topLevelDecl.get().getSingleDecl(), true);
      }
    }
  }
  if (listener) {
    listener->OnDone();
  }
}
