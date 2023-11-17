#include "stone/Parse/Parser.h"
#include "stone/Public.h"
#include "stone/Syntax/ASTContext.h"
#include "stone/Syntax/Module.h"

using namespace stone;

void stone::ParseSourceFile(SourceFile &sourceFile, ASTContext &syntaxContext,
                         SyntaxListener *syntaxListener,
                         LexerListener *lexerListener) {

  Parser parser(sourceFile, syntaxContext, syntaxListener, lexerListener);
  llvm::SmallVector<SyntaxResult<Decl>> results;
  parser.ParseTopLevelDecls(results);

  if (parser.HasError()) {
    return;
  }
  for (auto result : results) {
    if (result.IsNull()) {
      return;
    }
    sourceFile.AddTopLevelDecl(result.Get());
  }
}
