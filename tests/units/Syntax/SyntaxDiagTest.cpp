#include "stone/Core/Context.h"
#include "stone/Core/CompileDiagnostic.h"
#include "stone/Core/Defer.h"
#include "stone/Core/DiagnosticEngine.h"
#include "stone/Core/SyntaxDiagnostic.h"
#include "stone/Core/TextDiagnosticListener.h"
#include "stone/Syntax/Decl.h"
#include "stone/Syntax/Syntax.h"
#include "stone/Syntax/SyntaxDiagnosticArgument.h"

using stone::diag::DeclArgument;
using stone::syn::Syntax;
using stone::syn::SyntaxContext;

#include "gtest/gtest.h"

using namespace stone;

class SyntaxDiagTest : public ::testing::Test {
protected:
  Context ctx;
  SearchPathOptions pathOpts;
  Syntax syntax;
  SrcMgr sm;

public:
  SyntaxDiagTest() : syntax(std::make_unique<SyntaxContext>(ctx, pathOpts)) {}
};

TEST_F(SyntaxDiagTest, PrintD) {

  STONE_DEFER { ctx.GetDiagEngine().Finish(); };

  ctx.GetDiagOptions().useColor = true;

  // Setup the dianostics formatter and emitter
  SyntaxDiagnosticFormatter formatter;

  TextDiagnosticEmitter emitter;
  emitter.SetFormatter(&formatter);

  TextDiagnosticListener textListener;
  textListener.SetEmitter(&emitter);

  ctx.GetDiagEngine().AddListener(textListener);

  syntax
      .PrintD(SrcLoc(), diag::note_prev_decl_def,
                      diag::DeclArgument(nullptr))
      .WithFix()
      .Replace(SrcLoc(), llvm::StringRef());

  syntax
      .PrintD(SrcLoc(), diag::err_case_stmt_without_body,
                      diag::BoolArgument(false))
      .WithFix()
      .Replace(SrcLoc(), llvm::StringRef());
}
