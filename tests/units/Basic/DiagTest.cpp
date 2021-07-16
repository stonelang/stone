#include "stone/Basic/DiagnosticEngine.h"
#include "stone/Basic/Basic.h"

#include "gtest/gtest.h"

using namespace stone;

class DiagTest : public ::testing::Test {
	Basic basic; 

public:
};

TEST_F(DiagTest, ErrorTest) {

	auto diagEngine = basic.GetDiagEngine(); 
}
