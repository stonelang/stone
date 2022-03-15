set(LLVM_LINK_COMPONENTS
  Core
  Support
)

add_stone_unittest(StoneCoreUnitTests
	BuiltinTest.cpp
  DiagTest.cpp
	FileMgrTest.cpp
	SrcMgrTest.cpp
)
target_link_libraries(StoneCoreUnitTests
  PRIVATE
	StoneCore
)

