
#include "stone/Basic/Basic.h"
#include "stone/Basic/LangOptions.h"
#include "stone/Basic/Malloc.h"
#include "stone/Basic/SrcMgr.h"
#include "gtest/gtest.h"

using namespace stone;

class CompileTest : public ::testing::Test {
protected:
  Basic basic;
};

TEST_F(CompileTest, CompileFile) {}
