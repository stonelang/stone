#include "stone/Core/Basic.h"
#include "stone/Core/CompileDiagnostic.h"
#include "stone/Core/DiagnosticEngine.h"
#include "stone/Core/SynDiagnostic.h"
#include "stone/Core/TextDiagnosticListener.h"
#include "stone/Syntax/SyntaxDiagArgument.h"

using stone::diag::DeclArgument;

#include "gtest/gtest.h"

using namespace stone;

class DiagTest : public ::testing::Test {
public:
  Basic basic;

public:
};

TEST_F(DiagTest, BasicTest) {

  basic.GetDiagOptions().useColor = true;

  TextDiagnosticListener textListener;
  basic.GetDiagEngine().AddListener(textListener);

  // basic.GetDiagEngine().Diagnose(diag::err_no_compile_args)
  //     << "test with no 'SrcLoc'";

  // basic.GetDiagEngine().Diagnose(SrcLoc(), diag::err_no_compile_args)
  //     << "test with blank 'SrcLoc'";

  basic.GetDiagEngine()
      .Diagnose(SrcLoc(), diag::note_prev_decl_def, diag::DeclArgument(nullptr))
      .WithFix()
      .Replace(SrcLoc(), llvm::StringRef());
}
