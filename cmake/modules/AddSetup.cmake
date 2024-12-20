include(GNUInstallPackageDir)
include(ExtendPath)
include(LLVMDistributionSupport)
include(FindPrefixFromConfig)


file(GLOB_RECURSE
  ALL_CXX_SOURCE_FILES
  *.[chi]pp *.[chi]xx *.cpp *.cc *.h *.hh *.[CHI]
)


macro(stone_init product)

#LLVM
#set(PATH_TO_LLVM_SOURCE "${CMAKE_SOURCE_DIR}")
#set(PATH_TO_LLVM_BUILD "${CMAKE_BINARY_DIR}")
#set(LLVM_PACKAGE_VERSION ${PACKAGE_VERSION})
#set(LLVM_CMAKE_DIR "${CMAKE_SOURCE_DIR}/cmake/modules")
#set(${product}_NATIVE_LLVM_TOOLS_PATH "${CMAKE_BINARY_DIR}/bin")

#CLANG
#set(PATH_TO_CLANG_BUILD "${CMAKE_BINARY_DIR}")
#set(CLANG_MAIN_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/tools/clang/include")
#set(CLANG_BUILD_INCLUDE_DIR "${CMAKE_BINARY_DIR}/tools/clang/include")

#STONE
#set(STONE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
#set(STONE_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}")
#set(STONE_CMAKE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")
#set(STONE_MAIN_INCLUDE_DIR "${STONE_SOURCE_DIR}/include")
#set(STONE_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/include")
#set(STONE_LIB_DIR "${CMAKE_BINARY_DIR}/${CMAKE_CFG_INTDIR}/lib/stone")
#set(STONE_STATIC_LIB_DIR "${CMAKE_BINARY_DIR}/${CMAKE_CFG_INTDIR}/lib/stone_static")



endmacro()

macro(stone_install)

  if (NOT LLVM_INSTALL_TOOLCHAIN_ONLY)
  install(DIRECTORY include/stone 
    DESTINATION include
    FILES_MATCHING
    PATTERN "*.def"
    PATTERN "*.h"
    PATTERN "config.h" EXCLUDE
  )

  install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/stone
    DESTINATION include
    FILES_MATCHING
    PATTERN "CMakeFiles" EXCLUDE
    PATTERN "*.inc"
    PATTERN "*.h"
    )

  install(PROGRAMS utils/bash-autocomplete.sh
    DESTINATION share/stone
    )

endif()


macro(stone_setup product)

  stone_set_props(product)

endmacro()

