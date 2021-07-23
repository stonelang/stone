set(LLVM_LINK_COMPONENTS
  Core
  Support
)

add_stone_unittest(StoneCodeGenUnitTests

)
target_link_libraries(StoneCodeGenUnitTests
  PRIVATE
	StoneBasic
)

