set(LLVM_LINK_COMPONENTS
  Core
  Support
)

add_stone_unittest(StoneSemanticsUnitTests
	TypeCheckTest.cpp

)
target_link_libraries(StoneSemanticsUnitTests
  PRIVATE
	StoneCompile
)

