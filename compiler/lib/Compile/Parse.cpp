#include "stone/CodeCompletionListener.h"
#include "stone/Parse/Parser.h"
#include "stone/Public.h"
#include "stone/AST/Module.h"
#include "stone/AST/ASTContext.h"

using namespace stone;
using namespace stone::ast;

void stone::ParseASTFile(ast::ASTFile &sf, ast::ASTContext &sc,
                            ASTListener *listener) {

  Parser parser(sf, sc, listener);
  llvm::SmallVector<ASTResult<Decl>> results;
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
