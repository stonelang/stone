#include "stone/Basic/Context.h"
#include "stone/Basic/Defer.h"
#include "stone/Basic/DiagnosticEngine.h"
#include "stone/Basic/FrontendDiagnostic.h"
#include "stone/Basic/SyntaxDiagnostic.h"
#include "stone/Basic/TextDiagnosticListener.h"
#include "stone/Syntax/Decl.h"
#include "stone/Syntax/Syntax.h"
#include "stone/Syntax/SyntaxDiagnosticArgument.h"

using stone::diag::Decl;
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

  // Setup the custom formatting to be able to handle syntax diagnostics
  auto diagFormatter = std::make_unique<SyntaxDiagnosticFormatter>();
  auto diagEmitter =
      std::make_unique<TextDiagnosticEmitter>(std::move(diagFormatter));

  TextDiagnosticListener diagListener(std::move(diagEmitter));

  ctx.GetDiagEngine().AddListener(diagListener);

  syntax.PrintD(SrcLoc(), diag::note_prev_decl_def, diag::Decl(nullptr))
      .WithFix()
      .Replace(SrcLoc(), llvm::StringRef());

  syntax.PrintD(SrcLoc(), diag::err_case_stmt_without_body, diag::Bool(false))
      .WithFix()
      .Replace(SrcLoc(), llvm::StringRef());
}
