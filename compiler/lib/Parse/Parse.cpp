#include "stone/Core.h"
#include "stone/Parse/Parser.h"
#include "stone/Syntax/ASTContext.h"
#include "stone/Syntax/Module.h"

using namespace stone;

// TODO:
//  llvm::ArrayRef<Decl *> SourceFile::GetTopLevelDecls() const {
//    auto &astContext = GetASTContext();
//    auto *mutableThis = const_cast<SourceFile *>(this);
//  }

void stone::ParseSourceFile(SourceFile &sourceFile, ASTContext &astContext,
                            CodeCompletionCallbacks *callbacks) {

  Parser parser(sourceFile, astContext);
  parser.SetCodeCompletionCallbacks(callbacks);

  llvm::SmallVector<SyntaxResult<Decl>> topLevelDecls;
  parser.ParseTopLevelDecls(topLevelDecls);
  if (parser.HasError()) {
    return;
  }
  for (auto topLevelDecl : topLevelDecls) {
    if (topLevelDecl.IsNull()) {
      return;
    }
    sourceFile.AddTopLevelDecl(topLevelDecl.Get());
  }
}

/// Dump the source file that we parsed
void stone::DumpSourceFile(SourceFile &sourceFile, ASTContext &astContext) {}
