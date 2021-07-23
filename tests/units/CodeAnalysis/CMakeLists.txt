set(LLVM_LINK_COMPONENTS
  Core
  Support
)

add_stone_unittest(StoneCodeAnalysisUnitTests
	LexerTest.cpp	
)
target_link_libraries(StoneCodeAnalysisUnitTests
  PRIVATE
	StoneBasic
)

