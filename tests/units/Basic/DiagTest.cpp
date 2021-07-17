#include "stone/Basic/Basic.h"
#include "stone/Basic/CompileDiagnostic.h"
#include "stone/Basic/DiagnosticEngine.h"

#include "gtest/gtest.h"

using namespace stone;

class DiagTest : public ::testing::Test {
public:
  Basic basic;

public:
};

TEST_F(DiagTest, BasicTest) {

  basic.GetDiagOptions().useColor = true;

  basic.GetDiagEngine().Diagnose(diag::err_no_compile_args)
      << "test with no 'SrcLoc'";

  basic.GetDiagEngine().Diagnose(SrcLoc(), diag::err_no_compile_args)
      << "test with blank 'SrcLoc'";
}
