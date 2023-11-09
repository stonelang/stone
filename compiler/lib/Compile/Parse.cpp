#include "stone/AST/ASTContext.h"
#include "stone/AST/Module.h"
#include "stone/Compile/Parser.h"
#include "stone/CodeCompletionListener.h"
#include "stone/Lang.h"

using namespace stone;

void Lang::ParseASTFile(stone::ASTFile &sf, stone::ASTContext &sc,
                        ASTListener *listener) {

  Parser parser(sf, sc, listener);
  llvm::SmallVector<ParserResult<Decl>> results;
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
