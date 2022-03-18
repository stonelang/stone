set(LLVM_LINK_COMPONENTS
  Core
  Support
)

add_stone_unittest(StoneDriverUnitTests
  DriverTest.cpp
)
target_link_libraries(StoneDriverUnitTests
  PRIVATE
  StoneDriver
)