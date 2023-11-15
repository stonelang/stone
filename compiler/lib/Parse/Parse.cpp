#include "stone/Parse/Parser.h"
#include "stone/Public.h"
#include "stone/Syntax/ASTContext.h"
#include "stone/Syntax/Module.h"

using namespace stone;
using namespace stone::syn;

void stone::ParseSyntaxFile(syn::SyntaxFile &syntaxFile,
                            syn::ASTContext &syntaxContext,
                            SyntaxListener *syntaxListener,
                            LexerListener *lexerListener) {

  Parser parser(syntaxFile, syntaxContext, syntaxListener, lexerListener);
  llvm::SmallVector<SyntaxResult<Decl>> results;
  parser.ParseTopLevelDecls(results);

  if (parser.HasError()) {
    return;
  }
  for (auto result : results) {
    if (result.IsNull()) {
      return;
    }
    syntaxFile.AddTopLevelDecl(result.Get());
  }
}
