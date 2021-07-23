set(LLVM_LINK_COMPONENTS
  Core
  Support
)

add_stone_unittest(StoneCodeAnalysisUnitTests
	LexerTest.cpp
	ParserTest.cpp
	TypeCheckTest.cpp

)
target_link_libraries(StoneCodeAnalysisUnitTests
  PRIVATE
	StoneCodeAnalysis
)

