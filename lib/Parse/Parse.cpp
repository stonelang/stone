#include "stone/Parse/Parse.h"
#include "stone/Basic/Ret.h"
#include "stone/Parse/Parser.h"
#include "stone/Parse/SyntaxListener.h"
#include "stone/Syntax/Module.h"
#include "stone/Syntax/Syntax.h"

using namespace stone;
using namespace stone::syn;

void syn::ParseSyntaxFile(SyntaxFile &sf, Syntax &syntax,
                          SyntaxListener *listener) {

  Parser parser(sf, syntax, listener);
  syn::DeclGroupPtrTy topDecl;
  while (true) {
    // Check for tk::eof
    if (parser.IsDone()) {
      if (listener) {
        listener->OnDone();
      }
      break;
    }
    // Check for errors from diag and if there are then exit.
    if (parser.HasError()) {
      if (listener) {
        listener->OnError();
      }
      break;
    }
    // Go through all of the top level decls in the file one at a time
    // As you process a decl, it will be added to the SyntaxFile
    if (parser.ParseTopDecl(topDecl)) {
      // Notifify that a top decl has been parsed.
      if (listener) {
        listener->OnDecl(topDecl.get().getSingleDecl(), true);
      }
    }
  }
}
