set(LLVM_LINK_COMPONENTS
  Core
  Support
)

add_stone_unittest(StoneGenUnitTests
	CodeGenTest.cpp
)
target_link_libraries(StoneGenUnitTests
  PRIVATE
	StoneBasic
)

