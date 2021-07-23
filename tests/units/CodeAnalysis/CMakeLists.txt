set(LLVM_LINK_COMPONENTS
  Core
  Support
)

add_stone_unittest(StoneCodeAnalysisUnitTests
	TypeCheckTest.cpp	
)
target_link_libraries(StoneCodeAnalysisUnitTests
  PRIVATE
	StoneCodeAnalysis
)

