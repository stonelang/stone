set(LLVM_LINK_COMPONENTS
  Core
  Support
)

add_stone_unittest(StoneCheckUnitTests
	TypeCheckTest.cpp
)
target_link_libraries(StoneCheckUnitTests
  PRIVATE
	StoneCompile
)

