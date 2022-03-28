#include "stone/Core/CompileDiagnostic.h"
#include "stone/Core/Context.h"
#include "stone/Core/DiagnosticEngine.h"
#include "stone/Core/SyntaxDiagnostic.h"
#include "stone/Core/TextDiagnosticListener.h"
#include "stone/Syntax/SyntaxDiagnosticArgument.h"

using stone::diag::DeclArgument;

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
