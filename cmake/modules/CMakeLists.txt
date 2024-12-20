include(GNUInstallPackageDir)
include(ExtendPath)
include(LLVMDistributionSupport)
include(FindPrefixFromConfig)

# Generate a list of CMake library targets so that other CMake projects can
# link against them. LLVM calls its version of this file LLVMExports.cmake, but
# the usual CMake convention seems to be ${Project}Targets.cmake.
set(STONE_INSTALL_PACKAGE_DIR "${CMAKE_INSTALL_PACKAGEDIR}/stone" CACHE STRING
  "Path for CMake subdirectory for Stone (defaults to '${CMAKE_INSTALL_PACKAGEDIR}/stone')")
# CMAKE_INSTALL_PACKAGEDIR might be absolute, so don't reuse below.
set(stone_cmake_builddir "${CMAKE_BINARY_DIR}/lib${LLVM_LIBDIR_SUFFIX}/cmake/stone")

# Keep this in sync with llvm/cmake/CMakeLists.txt!
set(LLVM_INSTALL_PACKAGE_DIR "${CMAKE_INSTALL_PACKAGEDIR}/llvm" CACHE STRING
  "Path for CMake subdirectory for LLVM (defaults to '${CMAKE_INSTALL_PACKAGEDIR}/llvm')")
# CMAKE_INSTALL_PACKAGEDIR might be absolute, so don't reuse below.
string(REPLACE "${CMAKE_CFG_INTDIR}" "." llvm_cmake_builddir "${LLVM_LIBRARY_DIR}")
set(llvm_cmake_builddir "${llvm_cmake_builddir}/cmake/llvm")

get_property(STONE_EXPORTS GLOBAL PROPERTY STONE_EXPORTS)
export(TARGETS ${STONE_EXPORTS} FILE ${stone_cmake_builddir}/StoneTargets.cmake)



#CLANG
#set(CLANG_SOURCE_DIR "${CMAKE_SOURCE_DIR}/tools/clang/include")
#set(CLANG_BINARY_DIR_DIR "${CMAKE_BINARY_DIR}/tools/clang/include")

#STONE
#set(STONE_SOURCE_DIR  "${CMAKE_CURRENT_SOURCE_DIR}")
#set(STONE_BINARY_DIR  "${CMAKE_CURRENT_BINARY_DIR}")
#set(STONE_CMAKE_DIR   "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")
#set(STONE_INCLUDE_DIR "${STONE_SOURCE_DIR}/include")



# Generate StoneConfig.cmake for the build tree.
set(STONE_CONFIG_CMAKE_DIR "${stone_cmake_builddir}")
set(STONE_CONFIG_LLVM_CMAKE_DIR "${llvm_cmake_builddir}")
set(STONE_CONFIG_INCLUDE_EXPORTS "include(\"${stone_cmake_builddir}/StoneTargets.cmake\")")
set(STONE_CONFIG_INCLUDE_DIRS
  "${STONE_SOURCE_DIR}/include"
  "${STONE_BINARY_DIR}/include"
  )
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/StoneConfig.cmake.in
  ${stone_cmake_builddir}/StoneConfig.cmake
  @ONLY)
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/StoneConfigVersion.cmake.in
  ${stone_cmake_builddir}/StoneConfigVersion.cmake
  @ONLY)
set(STONE_CONFIG_CMAKE_DIR)
set(STONE_CONFIG_LLVM_CMAKE_DIR)

# For compatibility with projects that include(StoneConfig)
# via CMAKE_MODULE_PATH, place API modules next to it.
# Copy without source permissions because the source could be read-only,
# but we need to write into the copied folder.
file(COPY .
  DESTINATION ${stone_cmake_builddir}
  NO_SOURCE_PERMISSIONS
  FILES_MATCHING PATTERN *.cmake
  PATTERN CMakeFiles EXCLUDE
  )

# Generate StoneConfig.cmake for the install tree.
find_prefix_from_config(STONE_CONFIG_CODE STONE_INSTALL_PREFIX "${STONE_INSTALL_PACKAGE_DIR}")
extend_path(STONE_CONFIG_CMAKE_DIR "\${STONE_INSTALL_PREFIX}" "${STONE_INSTALL_PACKAGE_DIR}")
extend_path(STONE_CONFIG_LLVM_CMAKE_DIR "\${STONE_INSTALL_PREFIX}" "${LLVM_INSTALL_PACKAGE_DIR}")
get_config_exports_includes(Stone STONE_CONFIG_INCLUDE_EXPORTS)
extend_path(base_includedir "\${STONE_INSTALL_PREFIX}" "${CMAKE_INSTALL_INCLUDEDIR}")
set(STONE_CONFIG_INCLUDE_DIRS
  "${base_includedir}"
  )
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/StoneConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/StoneConfig.cmake
  @ONLY)
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/StoneConfigVersion.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/StoneConfigVersion.cmake
  @ONLY)
set(STONE_CONFIG_CODE)
set(STONE_CONFIG_CMAKE_DIR)

if (NOT LLVM_INSTALL_TOOLCHAIN_ONLY)
  install_distribution_exports(Stone)

  install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/StoneConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/StoneConfigVersion.cmake
    ${CMAKE_CURRENT_SOURCE_DIR}/AddStone.cmake
    DESTINATION ${STONE_INSTALL_PACKAGE_DIR}
    COMPONENT stone-cmake-exports)

  if(NOT LLVM_ENABLE_IDE)
    # Add a dummy target so this can be used with LLVM_DISTRIBUTION_COMPONENTS
    add_custom_target(stone-cmake-exports)
    add_llvm_install_targets(install-stone-cmake-exports
                             COMPONENT stone-cmake-exports)
  endif()
endif()
