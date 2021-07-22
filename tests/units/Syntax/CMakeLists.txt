set(LLVM_LINK_COMPONENTS
  Core
  Support
)

add_stone_unittest(StoneSyntaxUnitTests

	SyntaxDiagTest.cpp
)
target_link_libraries(StoneSyntaxUnitTests
  PRIVATE
	StoneSyntax
)

