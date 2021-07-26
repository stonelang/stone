set(LLVM_LINK_COMPONENTS
  Core
  Support
)

add_stone_unittest(StoneParseUnitTests
	LexerTest.cpp
	ParserTest.cpp

)
target_link_libraries(StoneParseUnitTests
  PRIVATE
	StoneParse
)

