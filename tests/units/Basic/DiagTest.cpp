#include "stone/Basic/FrontendDiagnostic.h"
#include "stone/Basic/Context.h"
#include "stone/Basic/DiagnosticEngine.h"
#include "stone/Basic/SyntaxDiagnostic.h"
#include "stone/Basic/TextDiagnosticListener.h"
#include "stone/Syntax/SyntaxDiagnosticArgument.h"

using stone::diag::Decl;

#include "gtest/gtest.h"

using namespace stone;

class DiagTest : public ::testing::Test {
public:
  Context ctx;

public:
};

TEST_F(DiagTest, DiagnoseCompile) {
  ctx.GetDiagOptions().useColor = true;

  TextDiagnosticListener textListener;
  ctx.GetDiagEngine().AddListener(textListener);

  ctx.GetDiagEngine()
      .PrintD(SrcLoc(), diag::err_no_input_files)
      .WithFix()
      .Replace(SrcLoc(), llvm::StringRef());
}
