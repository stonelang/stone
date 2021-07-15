#include "stone/Basic/Ret.h"
#include "stone/Compile/Parse.h"
#include "stone/Compile/LexerPipeline.h"
#include "stone/Compile/Parser.h"
#include "stone/Compile/ParserPipeline.h"
#include "stone/Syntax/Module.h"
#include "stone/Syntax/Syntax.h"

using namespace stone;
using namespace stone::syn;

void syn::ParseSyntaxFile(SyntaxFile &sf, Syntax &syntax, bool check,
                          PipelineEngine *pe) {

  ParserPipeline *pp = nullptr;
  LexerPipeline *lp = nullptr;

  Parser parser(sf, syntax, check);

  if (pe) {
    if (pe->Get(PipelineType::Parse)) {
      pp = static_cast<ParserPipeline *>(pe->Get(PipelineType::Parse));
      if (pp) {
        parser.SetPipeline(pp);
      }
    }
    if (pe->Get(PipelineType::Lex)) {
      lp = static_cast<LexerPipeline *>(pe->Get(PipelineType::Lex));
      if (lp) {
        parser.GetLexer().SetPipeline(lp);
      }
    }
  }

  syn::DeclGroupPtrTy topDecl;
  while (true) {
    // Check for tk::eof
    if (parser.IsDone()) {
      if (pp) {
        pp->OnDone();
      }
      break;
    }
    // Check for errors from diag and if there are then exit.
    if (parser.HasError()) {
      if (pp) {
        pp->OnError();
      }
      break;
    }
    // Go through all of the top level decls in the file one at a time
    // As you process a decl, it will be added to the SyntaxFile
    if (parser.ParseTopDecl(topDecl)) {
      // Notifify that a top decl has been parsed.
      if (pp) {
        pp->OnTopDecl(topDecl.get().getSingleDecl());
      }
    }
  }
}
