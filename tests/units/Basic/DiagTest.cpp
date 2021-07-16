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

TEST_F(DiagTest, ErrorTest) {

  basic.GetDiagOptions().useColor = true;
  basic.GetDiagEngine().Issue(SrcLoc(), diag::compile_error_no_compile_args)
      << "test";
}
