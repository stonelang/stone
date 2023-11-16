#include "stone/Parse/Parser.h"
#include "stone/Public.h"
#include "stone/Syntax/ASTContext.h"
#include "stone/Syntax/Module.h"

using namespace stone;

void stone::ParseASTFile(ASTFile &astFile, ASTContext &syntaxContext,
                         SyntaxListener *syntaxListener,
                         LexerListener *lexerListener) {

  Parser parser(astFile, syntaxContext, syntaxListener, lexerListener);
  llvm::SmallVector<SyntaxResult<Decl>> results;
  parser.ParseTopLevelDecls(results);

  if (parser.HasError()) {
    return;
  }
  for (auto result : results) {
    if (result.IsNull()) {
      return;
    }
    astFile.AddTopLevelDecl(result.Get());
  }
}
