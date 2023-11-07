#include "stone/Basic/CompilerDiagnostic.h"
#include "stone/Basic/DiagnosticEngine.h"
#include "stone/Basic/SyntaxDiagnostic.h"
#include "stone/Basic/TextDiagnosticFormatter.h"
#include "stone/Basic/TextDiagnosticListener.h"
#include "stone/Lang.h"
#include "stone/Syntax/SyntaxDiagnosticArgument.h"

using stone::diag::Decl;

#include "gtest/gtest.h"

using namespace stone;

class DiagTest : public ::testing::Test {
public:
  Lang ctx;

public:
};

TEST_F(DiagTest, DiagnoseCompile) {

  ctx.GetDiagUnit().GetDiagOptions().useColor = true;

  SyntaxDiagnosticFormatter diagFormatter;
  SyntaxDiagnosticEmitter diagEmitter(diagFormatter);
  TextDiagnosticListener diagListener(diagEmitter);

  ctx.GetDiagUnit().GetDiagEngine().AddListener(diagListener);

  ctx.GetDiagUnit()
      .PrintD(SrcLoc(), diag::err_no_input_files)
      .WithFix()
      .Replace(SrcLoc(), llvm::StringRef());

  ctx.GetDiagUnit().GetDiagEngine().Finish();
}
