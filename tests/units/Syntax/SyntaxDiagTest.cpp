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
using stone::syn::TreeContext;

#include "gtest/gtest.h"

using namespace stone;

class SyntaxDiagTest : public ::testing::Test {
protected:
  Context ctx;
  SearchPathOptions pathOpts;
  Syntax syntax;
  SrcMgr sm;
  TreeContext tc;

public:
  SyntaxDiagTest() : tc(ctx, pathOpts), syntax(tc) {}
};

TEST_F(SyntaxDiagTest, DiagnoseSyntax) {

  STONE_DEFER { ctx.GetDiagEngine().Finish(); };

  ctx.GetDiagOptions().useColor = true;

  TextDiagnosticListener textListener;
  ctx.GetDiagEngine().AddListener(textListener);

  syntax
      .DiagnoseSyntax(SrcLoc(), diag::note_prev_decl_def,
                      diag::DeclArgument(nullptr))
      .WithFix()
      .Replace(SrcLoc(), llvm::StringRef());

  syntax
      .DiagnoseSyntax(SrcLoc(), diag::err_case_stmt_without_body,
                      diag::BoolArgument(false))
      .WithFix()
      .Replace(SrcLoc(), llvm::StringRef());
}
