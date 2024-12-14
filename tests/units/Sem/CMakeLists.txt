set(LLVM_LINK_COMPONENTS
  Core
  Support
)

add_stone_unittest(StoneSemUnitTests
	TypeCheckTest.cpp
)
target_link_libraries(StoneSemUnitTests
  PRIVATE
	StoneCompile
)

