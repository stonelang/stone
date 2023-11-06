#include "stone/AST/ASTContext.h"
#include "stone/AST/Module.h"
#include "stone/CodeCompletionListener.h"
#include "stone/Compile/Parser.h"
#include "stone/Lang.h"

using namespace stone;
using namespace stone::ast;



Status CompilerInstance::CompileWithParsing() {
  return CompileWithParsing([&](ast::ASTFile &) { return Status::Success(); });
}

Status CompilerInstance::CompileWithParsing(ParsingCompletedCallback notifiy) {

  for (auto moduleFile : GetModuleSystem().GetMainModule()->GetFiles()) {
    if (auto *astFile = llvm::dyn_cast<ast::ASTFile>(moduleFile)) {
      Lang::ParseASTFile(*asttaxFile, GetASTContext(),
                         invocation.GetListener());
      if (notifiy) {
        notifiy(*astFile);
      }
    }
  }

  if (!GetMode().JustParse()) {
    ResolveImports();
  }
  if (invocation.GetListener()) {
    invocation.GetListener()->OnASTAnalysisCompleted(*this);
  }
  return Status::Success();
}

void Lang::ParseASTFile(ast::ASTFile &sf, ast::ASTContext &sc,
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
