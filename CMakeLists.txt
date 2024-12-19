cmake_minimum_required(VERSION 3.15)
project(stone)

# Make sure that our source directory is on the current cmake module path so that
# we can include cmake files from this directory.
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

include(CMakeParseArguments)

# Make sure to include and invoke properties first 
include(StoneProperties)
set_stone_properties(STONE)

# Now, we can include these
include(StoneCore)
include(StoneTblGen)
include(StoneFormat)
include(StoneUnitTests)

set(CMAKE_INCLUDE_CURRENT_DIR ON)


add_subdirectory(utils/stone-tblgen)


include_directories(BEFORE
  ${CMAKE_CURRENT_BINARY_DIR}/compiler/include
  ${CMAKE_CURRENT_SOURCE_DIR}/compiler/include
  ${CLANG_BUILD_INCLUDE_DIR}
  ${CLANG_MAIN_INCLUDE_DIR}
)


set_stone_version()
message(STATUS "Stone version: ${STONE_VERSION}")


# Install the path for the lib files 
install_stone()


add_definitions( -D_GNU_SOURCE )

option(STONE_BUILD_TOOLS
  "Build the stone tools. If OFF, just generate build targets." ON)

add_subdirectory(compiler)
add_subdirectory(tools)
#add_subdirectory(tests)



