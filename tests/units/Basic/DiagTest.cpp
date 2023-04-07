#include "stone/Diag/CompilerDiagnostic.h"
#include "stone/Diag/DiagnosticEngine.h"
#include "stone/Diag/SyntaxDiagnostic.h"
#include "stone/Diag/TextDiagnosticListener.h"
#include "stone/LangContext.h"
#include "stone/Syntax/SyntaxDiagnosticArgument.h"

using stone::diag::Decl;

#include "gtest/gtest.h"

using namespace stone;

class DiagTest : public ::testing::Test {
public:
  LangContext ctx;

public:
};

TEST_F(DiagTest, DiagnoseCompile) {
  ctx.GetDiagUnit().GetDiagOptions().useColor = true;

  // TextDiagnosticListener textListener;
  // ctx.GetDiagUnit().GetDiagEngine().AddListener(textListener);

  // ctx.GetDiagUnit()
  //     .PrintD(SrcLoc(), diag::err_no_input_files)
  //     .WithFix()
  //     .Replace(SrcLoc(), llvm::StringRef());
  
}
