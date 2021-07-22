#include "stone/Basic/Basic.h"
#include "stone/Basic/CompileDiagnostic.h"
#include "stone/Basic/Defer.h"
#include "stone/Basic/DiagnosticEngine.h"
#include "stone/Basic/SyntaxDiagnostic.h"
#include "stone/Basic/TextDiagnosticListener.h"
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
  Basic basic;
  SearchPathOptions pathOpts;
  Syntax syntax;
  SrcMgr sm;
  TreeContext tc;

public:
  SyntaxDiagTest()
      : sm(basic.GetDiagEngine(), basic.GetFileMgr()), tc(basic, pathOpts, sm),
        syntax(tc) {}
};

TEST_F(SyntaxDiagTest, DiagnoseSyntax) {

  STONE_DEFER { basic.GetDiagEngine().Finish(); };

  basic.GetDiagOptions().useColor = true;

  TextDiagnosticListener textListener;
  basic.GetDiagEngine().AddListener(textListener);

  syntax
      .DiagnoseSyntax(SrcLoc(), diag::note_prev_decl_def,
                      diag::DeclArgument(nullptr))
      .WithFix()
      .Replace(SrcLoc(), llvm::StringRef());
}
