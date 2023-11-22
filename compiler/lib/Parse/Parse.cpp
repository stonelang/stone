#include "stone/Parse/Parser.h"
#include "stone/Public.h"
#include "stone/Syntax/ASTContext.h"
#include "stone/Syntax/Module.h"

using namespace stone;

// TODO:
//  llvm::ArrayRef<Decl *> SourceFile::GetTopLevelDecls() const {
//    auto &astContext = GetASTContext();
//    auto *mutableThis = const_cast<SourceFile *>(this);
//  }

void stone::ParseSourceFile(SourceFile &sourceFile, ASTContext &syntaxContext,
                            SyntaxListener *syntaxListener,
                            LexerListener *lexerListener) {

  Parser parser(sourceFile, syntaxContext);
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
