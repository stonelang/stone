set(LLVM_LINK_COMPONENTS
  Core
  Support
)

add_stone_unittest(StoneLexUnitTests
  LexerTest.cpp
)
target_link_libraries(StoneLexUnitTests
  PRIVATE
	StoneCompile
)

