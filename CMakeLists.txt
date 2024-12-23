cmake_minimum_required(VERSION 3.20.0)
set(LLVM_SUBPROJECT_TITLE "Stone")

if(NOT DEFINED LLVM_COMMON_CMAKE_UTILS)
  set(LLVM_COMMON_CMAKE_UTILS ${CMAKE_CURRENT_SOURCE_DIR}/../cmake)
endif()
include(${LLVM_COMMON_CMAKE_UTILS}/Modules/CMakePolicy.cmake
  NO_POLICY_SCOPE)

# If we are not building as a part of LLVM, build Stone as an
# standalone project, using LLVM as an external library:
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  project(Stone)
  set(STONE_BUILT_STANDALONE TRUE)
endif()

# Make sure that our source directory is on the current cmake module path so that
# we can include cmake files from this directory.
list(INSERT CMAKE_MODULE_PATH 0
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules"
  "${LLVM_COMMON_CMAKE_UTILS}/Modules"
  )

# Must go below project(..)
include(GNUInstallDirs)
#include(GetDarwinLinkerVersion)


set(STONE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(STONE_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})


if(STONE_BUILT_STANDALONE)
  set(CMAKE_CXX_STANDARD 17 CACHE STRING "C++ standard to conform to")
  set(CMAKE_CXX_STANDARD_REQUIRED YES)
  set(CMAKE_CXX_EXTENSIONS NO)

  if(NOT MSVC_IDE)
    set(LLVM_ENABLE_ASSERTIONS ${ENABLE_ASSERTIONS}
      CACHE BOOL "Enable assertions")
    # Assertions should follow llvm-config's.
    mark_as_advanced(LLVM_ENABLE_ASSERTIONS)
  endif()

  find_package(LLVM REQUIRED HINTS "${LLVM_CMAKE_DIR}")
  list(APPEND CMAKE_MODULE_PATH "${LLVM_DIR}")

  # Turn into CACHE PATHs for overwritting
  set(LLVM_INCLUDE_DIRS ${LLVM_INCLUDE_DIRS} CACHE PATH "Path to llvm/include and any other header dirs needed")
  set(LLVM_BINARY_DIR "${LLVM_BINARY_DIR}" CACHE PATH "Path to LLVM build tree")
  set(LLVM_MAIN_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../llvm" CACHE PATH "Path to LLVM source tree")
  set(LLVM_TOOLS_BINARY_DIR "${LLVM_TOOLS_BINARY_DIR}" CACHE PATH "Path to llvm/bin")
  set(LLVM_LIBRARY_DIR "${LLVM_LIBRARY_DIR}" CACHE PATH "Path to llvm/lib")

  find_program(LLVM_TABLEGEN_EXE "llvm-tblgen" ${LLVM_TOOLS_BINARY_DIR}
    NO_DEFAULT_PATH)

  # They are used as destination of target generators.
  set(LLVM_RUNTIME_OUTPUT_INTDIR ${CMAKE_BINARY_DIR}/${CMAKE_CFG_INTDIR}/bin)
  set(LLVM_LIBRARY_OUTPUT_INTDIR ${CMAKE_BINARY_DIR}/${CMAKE_CFG_INTDIR}/lib${LLVM_LIBDIR_SUFFIX})
  if(WIN32 OR CYGWIN)
    # DLL platform -- put DLLs into bin.
    set(LLVM_SHLIB_OUTPUT_INTDIR ${LLVM_RUNTIME_OUTPUT_INTDIR})
  else()
    set(LLVM_SHLIB_OUTPUT_INTDIR ${LLVM_LIBRARY_OUTPUT_INTDIR})
  endif()

  option(LLVM_INSTALL_TOOLCHAIN_ONLY
    "Only include toolchain files in the 'install' target." OFF)

  option(LLVM_FORCE_USE_OLD_TOOLCHAIN
    "Set to ON to force using an old, unsupported host toolchain." OFF)
  option(STONE_ENABLE_BOOTSTRAP "Generate the stone bootstrap target" OFF)
  option(LLVM_ENABLE_LIBXML2 "Use libxml2 if available." ON)

  include(AddLLVM)
  include(TableGen)
  include(HandleLLVMOptions)
  include(VersionFromVCS)
  include(CheckAtomic)
  include(GetErrcMessages)
  include(LLVMDistributionSupport)

  set(PACKAGE_VERSION "${LLVM_PACKAGE_VERSION}")
  set(BUG_REPORT_URL "${LLVM_PACKAGE_BUGREPORT}" CACHE STRING
    "Default URL where bug reports are to be submitted.")

  if (NOT DEFINED LLVM_INCLUDE_TESTS)
    set(LLVM_INCLUDE_TESTS ON)
  endif()

  include_directories(${LLVM_INCLUDE_DIRS})
  link_directories("${LLVM_LIBRARY_DIR}")

  set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )
  set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib${LLVM_LIBDIR_SUFFIX} )
  set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib${LLVM_LIBDIR_SUFFIX} )

  find_package(Python3 ${LLVM_MINIMUM_PYTHON_VERSION} REQUIRED
    COMPONENTS Interpreter)

  if(LLVM_INCLUDE_TESTS)
    # Check prebuilt llvm/utils.
    if(EXISTS ${LLVM_TOOLS_BINARY_DIR}/FileCheck${CMAKE_EXECUTABLE_SUFFIX}
        AND EXISTS ${LLVM_TOOLS_BINARY_DIR}/count${CMAKE_EXECUTABLE_SUFFIX}
        AND EXISTS ${LLVM_TOOLS_BINARY_DIR}/not${CMAKE_EXECUTABLE_SUFFIX})
      set(LLVM_UTILS_PROVIDED ON)
    endif()

    # Seek installed Lit.
    find_program(LLVM_LIT
                 NAMES llvm-lit lit.py lit
                 PATHS "${LLVM_MAIN_SRC_DIR}/utils/lit"
                 DOC "Path to lit.py")

    if(EXISTS ${LLVM_MAIN_SRC_DIR}/utils/lit/lit.py)
      # Note: path not really used, except for checking if lit was found
      if(EXISTS ${LLVM_MAIN_SRC_DIR}/utils/llvm-lit)
        add_subdirectory(${LLVM_MAIN_SRC_DIR}/utils/llvm-lit utils/llvm-lit)
      endif()
      if(NOT LLVM_UTILS_PROVIDED)
        add_subdirectory(${LLVM_MAIN_SRC_DIR}/utils/FileCheck utils/FileCheck)
        add_subdirectory(${LLVM_MAIN_SRC_DIR}/utils/count utils/count)
        add_subdirectory(${LLVM_MAIN_SRC_DIR}/utils/not utils/not)
        set(LLVM_UTILS_PROVIDED ON)
        set(STONE_TEST_DEPS FileCheck count not)
      endif()
    endif()

    if (NOT TARGET llvm_gtest)
        message(FATAL_ERROR "llvm-gtest not found. Please install llvm-gtest or disable tests with -DLLVM_INCLUDE_TESTS=OFF")
    endif()

    if(LLVM_LIT)
      # Define the default arguments to use with 'lit', and an option for the user
      # to override.
      set(LIT_ARGS_DEFAULT "-sv")
      if (MSVC OR XCODE)
        set(LIT_ARGS_DEFAULT "${LIT_ARGS_DEFAULT} --no-progress-bar")
      endif()
      set(LLVM_LIT_ARGS "${LIT_ARGS_DEFAULT}" CACHE STRING "Default options for lit")

      get_errc_messages(LLVM_LIT_ERRC_MESSAGES)

      # On Win32 hosts, provide an option to specify the path to the GnuWin32 tools.
      if( WIN32 AND NOT CYGWIN )
        set(LLVM_LIT_TOOLS_DIR "" CACHE PATH "Path to GnuWin32 tools")
      endif()
    else()
      set(LLVM_INCLUDE_TESTS OFF)
    endif()

    umbrella_lit_testsuite_begin(check-all)
  endif() # LLVM_INCLUDE_TESTS
endif() # standalone

# This allows disabling stone's XML dependency even if LLVM finds libxml2.
# By default, stone depends on libxml2 if LLVM does.
option(STONE_ENABLE_LIBXML2 "Whether libstone may depend on libxml2"
       ${LLVM_ENABLE_LIBXML2})

if(STONE_ENABLE_LIBXML2)
  # Don't look for libxml if we're using MSan, since uninstrumented third party
  # code may call MSan interceptors like strlen, leading to false positives.
  if(NOT LLVM_USE_SANITIZER MATCHES "Memory.*")
    set (LIBXML2_FOUND 0)
    find_package(LibXml2 2.5.3 QUIET)
    if (LIBXML2_FOUND)
      set(STONE_HAVE_LIBXML 1)
    endif()
  endif()
endif()

if(STONE_ENABLE_CIR)
  if (STONE_BUILT_STANDALONE)
    message(FATAL_ERROR
      "StoneIR is not yet supported in the standalone build.")
  endif()
  if (NOT "${LLVM_ENABLE_PROJECTS}" MATCHES "MLIR|mlir")
    message(FATAL_ERROR
      "Cannot build StoneIR without MLIR in LLVM_ENABLE_PROJECTS")
  endif()
endif()

include(CheckIncludeFile)
check_include_file(sys/resource.h STONE_HAVE_RLIMITS)

# This check requires _GNU_SOURCE on linux
check_include_file(dlfcn.h STONE_HAVE_DLFCN_H)
if( STONE_HAVE_DLFCN_H )
  include(CheckLibraryExists)
  include(CheckSymbolExists)
  check_library_exists(dl dlopen "" HAVE_LIBDL)
  if( HAVE_LIBDL )
    list(APPEND CMAKE_REQUIRED_LIBRARIES dl)
  endif()
  list(APPEND CMAKE_REQUIRED_DEFINITIONS -D_GNU_SOURCE)
  check_symbol_exists(dladdr dlfcn.h STONE_HAVE_DLADDR)
  list(REMOVE_ITEM CMAKE_REQUIRED_DEFINITIONS -D_GNU_SOURCE)
  if( HAVE_LIBDL )
    list(REMOVE_ITEM CMAKE_REQUIRED_LIBRARIES dl)
  endif()
endif()

set(STONE_RESOURCE_DIR "" CACHE STRING
  "Relative directory from the Stone binary to its resource files.")

set(C_INCLUDE_DIRS "" CACHE STRING
  "Colon separated list of directories stone will search for headers.")

set(USE_DEPRECATED_GCC_INSTALL_PREFIX OFF CACHE BOOL "Temporary workaround before GCC_INSTALL_PREFIX is completely removed")
set(GCC_INSTALL_PREFIX "" CACHE PATH "Directory where gcc is installed." )
set(DEFAULT_SYSROOT "" CACHE STRING
  "Default <path> to all compiler invocations for --sysroot=<path>." )
if(GCC_INSTALL_PREFIX AND NOT USE_DEPRECATED_GCC_INSTALL_PREFIX)
  message(FATAL_ERROR "GCC_INSTALL_PREFIX is deprecated and will be removed. Use "
    "configuration files (https://stone.llvm.org/docs/UsersManual.html#configuration-files)"
    "to specify the default --gcc-install-dir= or --gcc-triple=. --gcc-toolchain= is discouraged. "
    "See https://github.com/llvm/llvm-project/pull/77537 for detail.")
endif()

set(ENABLE_LINKER_BUILD_ID OFF CACHE BOOL "pass --build-id to ld")

set(ENABLE_X86_RELAX_RELOCATIONS ON CACHE BOOL
    "enable x86 relax relocations by default")

set(PPC_LINUX_DEFAULT_IEEELONGDOUBLE OFF CACHE BOOL
    "Enable IEEE binary128 as default long double format on PowerPC Linux.")

set(STONE_SPAWN_CC1 OFF CACHE BOOL
    "Whether stone should use a new process for the CC1 invocation")

option(STONE_DEFAULT_PIE_ON_LINUX "Default to -fPIE and -pie on linux-gnu" ON)

set(STONE_DEFAULT_LINKER "" CACHE STRING
  "Default linker to use (linker name or absolute path, empty for platform default)")

set(STONE_DEFAULT_CXX_STDLIB "" CACHE STRING
  "Default C++ stdlib to use (\"libstdc++\" or \"libc++\", empty for platform default")
if (NOT(STONE_DEFAULT_CXX_STDLIB STREQUAL "" OR
        STONE_DEFAULT_CXX_STDLIB STREQUAL "libstdc++" OR
        STONE_DEFAULT_CXX_STDLIB STREQUAL "libc++"))
  message(WARNING "Resetting default C++ stdlib to use platform default")
  set(STONE_DEFAULT_CXX_STDLIB "" CACHE STRING
    "Default C++ stdlib to use (\"libstdc++\" or \"libc++\", empty for platform default" FORCE)
endif()

set(STONE_DEFAULT_RTLIB "" CACHE STRING
  "Default runtime library to use (\"libgcc\" or \"compiler-rt\", empty for platform default)")
if (NOT(STONE_DEFAULT_RTLIB STREQUAL "" OR
        STONE_DEFAULT_RTLIB STREQUAL "libgcc" OR
        STONE_DEFAULT_RTLIB STREQUAL "compiler-rt"))
  message(WARNING "Resetting default rtlib to use platform default")
  set(STONE_DEFAULT_RTLIB "" CACHE STRING
    "Default runtime library to use (\"libgcc\" or \"compiler-rt\", empty for platform default)" FORCE)
endif()

set(STONE_DEFAULT_UNWINDLIB "" CACHE STRING
  "Default unwind library to use (\"none\" \"libgcc\" or \"libunwind\", empty to match runtime library.)")
if (STONE_DEFAULT_UNWINDLIB STREQUAL "")
  if (STONE_DEFAULT_RTLIB STREQUAL "libgcc")
    set (STONE_DEFAULT_UNWINDLIB "libgcc" CACHE STRING "" FORCE)
  endif()
endif()

if (NOT(STONE_DEFAULT_UNWINDLIB STREQUAL "" OR
        STONE_DEFAULT_UNWINDLIB STREQUAL "none" OR
        STONE_DEFAULT_UNWINDLIB STREQUAL "libgcc" OR
        STONE_DEFAULT_UNWINDLIB STREQUAL "libunwind"))
  message(WARNING "Resetting default unwindlib to use platform default")
  set(STONE_DEFAULT_UNWINDLIB "" CACHE STRING
    "Default unwind library to use (\"none\" \"libgcc\" or \"libunwind\", empty to match runtime library.)" FORCE)
endif()

set(STONE_DEFAULT_OBJCOPY "objcopy" CACHE STRING
  "Default objcopy executable to use.")

set(STONE_DEFAULT_OPENMP_RUNTIME "libomp" CACHE STRING
  "Default OpenMP runtime used by -fopenmp.")

set(STONE_SYSTEMZ_DEFAULT_ARCH "z10" CACHE STRING "SystemZ Default Arch")

set(STONE_VENDOR ${PACKAGE_VENDOR} CACHE STRING
  "Vendor-specific text for showing with version information.")

set(STONE_REPOSITORY_STRING "" CACHE STRING
  "Vendor-specific text for showing the repository the source is taken from.")

if(STONE_REPOSITORY_STRING)
  add_definitions(-DSTONE_REPOSITORY_STRING="${STONE_REPOSITORY_STRING}")
endif()

set(STONE_VENDOR_UTI "org.llvm.stone" CACHE STRING
  "Vendor-specific uti.")

set(STONE_PYTHON_BINDINGS_VERSIONS "" CACHE STRING
    "Python versions to install libstone python bindings for")

set(STONE_LINK_STONE_DYLIB ${LLVM_LINK_LLVM_DYLIB} CACHE BOOL
    "Link tools against libstone-cpp.so")

if (NOT LLVM_LINK_LLVM_DYLIB AND STONE_LINK_STONE_DYLIB)
  message(FATAL_ERROR "Cannot set STONE_LINK_STONE_DYLIB=ON when "
                      "LLVM_LINK_LLVM_DYLIB=OFF")
endif()

# The libdir suffix must exactly match whatever LLVM's configuration used.
set(STONE_LIBDIR_SUFFIX "${LLVM_LIBDIR_SUFFIX}")

set(STONE_TOOLS_INSTALL_DIR "${CMAKE_INSTALL_BINDIR}" CACHE PATH
    "Path for binary subdirectory (defaults to '${CMAKE_INSTALL_BINDIR}')")
mark_as_advanced(STONE_TOOLS_INSTALL_DIR)


if( CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR AND NOT MSVC_IDE )
  message(FATAL_ERROR "In-source builds are not allowed. "
"Please create a directory and run cmake "
"from there, passing the path to this source directory as the last argument. "
"This process created the file `CMakeCache.txt' and the directory "
"`CMakeFiles'. Please delete them.")
endif()

# If STONE_VERSION_* is specified, use it, if not use LLVM_VERSION_*.
if(NOT DEFINED STONE_VERSION_MAJOR)
  set(STONE_VERSION_MAJOR ${LLVM_VERSION_MAJOR})
endif()
if(NOT DEFINED STONE_VERSION_MINOR)
  set(STONE_VERSION_MINOR ${LLVM_VERSION_MINOR})
endif()
if(NOT DEFINED STONE_VERSION_PATCHLEVEL)
  set(STONE_VERSION_PATCHLEVEL ${LLVM_VERSION_PATCH})
endif()
if(NOT DEFINED STONE_VERSION_SUFFIX)
  set(STONE_VERSION_SUFFIX ${LLVM_VERSION_SUFFIX})
endif()
set(STONE_VERSION "${STONE_VERSION_MAJOR}.${STONE_VERSION_MINOR}.${STONE_VERSION_PATCHLEVEL}${STONE_VERSION_SUFFIX}")
message(STATUS "Stone version: ${STONE_VERSION}")

# Configure the Version.inc file.
#configure_file(
#  ${CMAKE_CURRENT_SOURCE_DIR}/include/stone/Basic/Version.inc.in
#  ${CMAKE_CURRENT_BINARY_DIR}/include/stone/Basic/Version.inc)

# Add appropriate flags for GCC
if (LLVM_COMPILER_IS_GCC_COMPATIBLE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-common -Woverloaded-virtual")
  if (NOT "${CMAKE_CXX_COMPILER_ID}" MATCHES "Stone")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-strict-aliasing")
  endif ()

  # Enable -pedantic for Stone even if it's not enabled for LLVM.
  if (NOT LLVM_ENABLE_PEDANTIC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -Wno-long-long")
  endif ()

  if (CMAKE_CXX_COMPILER_ID MATCHES "Stone")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-nested-anon-types" )
  endif ()
endif ()

# Determine HOST_LINK_VERSION on Darwin.
set(HOST_LINK_VERSION)
if (APPLE AND NOT CMAKE_LINKER MATCHES ".*lld.*")
  #get_darwin_linker_version(HOST_LINK_VERSION)
  message(STATUS "Host linker version: ${HOST_LINK_VERSION}")
endif()

include(CMakeParseArguments)
include(AddStone)
include(AddFormat)

set(CMAKE_INCLUDE_CURRENT_DIR ON)


include_directories(BEFORE
  ${CMAKE_CURRENT_BINARY_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_BINARY_DIR}/tools/clang/include
  ${CMAKE_SOURCE_DIR}/tools/clang/include
  )

if (NOT LLVM_INSTALL_TOOLCHAIN_ONLY)
  install(DIRECTORY include/stone include/stone-c
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    COMPONENT stone-headers
    FILES_MATCHING
    PATTERN "*.def"
    PATTERN "*.h"
    PATTERN "config.h" EXCLUDE
    )

  install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/stone
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    COMPONENT stone-headers
    FILES_MATCHING
    PATTERN "CMakeFiles" EXCLUDE
    PATTERN "*.inc"
    PATTERN "*.h"
    )

  # Installing the headers needs to depend on generating any public
  # tablegen'd headers.
  add_custom_target(stone-headers DEPENDS stone-tablegen-targets)
  set_target_properties(stone-headers PROPERTIES FOLDER "Stone/Resources")
  if(NOT LLVM_ENABLE_IDE)
    add_llvm_install_targets(install-stone-headers
                             DEPENDS stone-headers
                             COMPONENT stone-headers)
  endif()

  #add_custom_target(bash-autocomplete DEPENDS utils/bash-autocomplete.sh)
  set_target_properties(bash-autocomplete PROPERTIES FOLDER "Stone/Misc")
  install(FILES utils/bash-autocomplete.sh
          DESTINATION "${CMAKE_INSTALL_DATADIR}/stone"
          COMPONENT bash-autocomplete)
  if(NOT LLVM_ENABLE_IDE)
    #add_llvm_install_targets(install-bash-autocomplete
    #                         DEPENDS bash-autocomplete
    #                         COMPONENT bash-autocomplete)
  endif()
endif()

option(STONE_BUILD_TOOLS
  "Build the Stone tools. If OFF, just generate build targets." ON)

if(LLVM_ENABLE_PLUGINS OR LLVM_EXPORT_SYMBOLS_FOR_PLUGINS)
  set(HAVE_STONE_PLUGIN_SUPPORT ON)
else()
  set(HAVE_STONE_PLUGIN_SUPPORT OFF)
endif()
CMAKE_DEPENDENT_OPTION(STONE_PLUGIN_SUPPORT
  "Build stone with plugin support" ON
  "HAVE_STONE_PLUGIN_SUPPORT" OFF)

# If libstdc++ is statically linked, stone-repl needs to statically link libstdc++
# itself, which is not possible in many platforms because of current limitations in
# JIT stack. (more platforms need to be supported by JITLink)
if(NOT LLVM_STATIC_LINK_CXX_STDLIB)
  set(HAVE_STONE_REPL_SUPPORT ON)
endif()

option(STONE_ENABLE_ARCMT "Build ARCMT." ON)
option(STONE_ENABLE_STATIC_ANALYZER
  "Include static analyzer in stone binary." ON)

option(STONE_ENABLE_PROTO_FUZZER "Build Stone protobuf fuzzer." OFF)

if(NOT STONE_ENABLE_STATIC_ANALYZER AND STONE_ENABLE_ARCMT)
  message(FATAL_ERROR "Cannot disable static analyzer while enabling ARCMT or Z3")
endif()

if(STONE_ENABLE_ARCMT)
  set(STONE_ENABLE_OBJC_REWRITER ON)
endif()

# This option is a stop-gap, we should commit to removing this as
# soon as possible. See discussion:
# https://discourse.llvm.org/t/rationale-for-removing-versioned-libstone-middle-ground-to-keep-it-behind-option/
option(STONE_FORCE_MATCHING_LIBSTONE_SOVERSION
  "Force the SOVERSION of libstone to be equal to STONE_MAJOR" ON)

# Stone version information
set(STONE_EXECUTABLE_VERSION
    "${STONE_VERSION_MAJOR}" CACHE STRING
    "Major version number that will be appended to the stone executable name")
set(LIBSTONE_LIBRARY_VERSION
    "${STONE_VERSION_MAJOR}" CACHE STRING
    "Major version number that will be appended to the libstone library")
mark_as_advanced(STONE_EXECUTABLE_VERSION LIBSTONE_LIBRARY_VERSION)

option(STONE_INCLUDE_TESTS
       "Generate build targets for the Stone unit tests."
       ${LLVM_INCLUDE_TESTS})

option(STONE_ENABLE_HLSL "Include HLSL build products" Off)
# While HLSL support is experimental this should stay hidden.
mark_as_advanced(STONE_ENABLE_HLSL)

#add_subdirectory(utils/stone-tablegen)

# Export STONE_TABLEGEN_EXE for use by flang docs.
set(STONE_TABLEGEN_EXE "${STONE_TABLEGEN_EXE}" CACHE INTERNAL "")

add_subdirectory(include)

# All targets below may depend on all tablegen'd files.
get_property(STONE_TABLEGEN_TARGETS GLOBAL PROPERTY STONE_TABLEGEN_TARGETS)
add_custom_target(stone-tablegen-targets
  DEPENDS
  omp_gen
  StoneOptions
  ${STONE_TABLEGEN_TARGETS})
set_target_properties(stone-tablegen-targets PROPERTIES FOLDER "Stone/Tablegenning/Targets")
list(APPEND LLVM_COMMON_DEPENDS stone-tablegen-targets)

# Force target to be built as soon as possible. Stone modules builds depend
# header-wise on it as they ship all headers from the umbrella folders. Building
# an entire module might include header, which depends on intrinsics_gen.
if(LLVM_ENABLE_MODULES)
  list(APPEND LLVM_COMMON_DEPENDS intrinsics_gen)
endif()

add_subdirectory(lib)
add_subdirectory(tools)
#add_subdirectory(runtime)

option(STONE_BUILD_EXAMPLES "Build STONE example programs by default." OFF)
#add_subdirectory(examples)

if(APPLE)
  # this line is needed as a cleanup to ensure that any CMakeCaches with the old
  # default value get updated to the new default.
  if(STONE_ORDER_FILE STREQUAL "")
    unset(STONE_ORDER_FILE CACHE)
    unset(STONE_ORDER_FILE)
  endif()


  set(STONE_ORDER_FILE ${CMAKE_CURRENT_BINARY_DIR}/stone.order CACHE FILEPATH
    "Order file to use when compiling stone in order to improve startup time (Darwin Only - requires ld64).")

  if(NOT EXISTS ${STONE_ORDER_FILE})
    string(FIND "${STONE_ORDER_FILE}" "${CMAKE_CURRENT_BINARY_DIR}" PATH_START)
    if(PATH_START EQUAL 0)
      file(WRITE ${STONE_ORDER_FILE} "\n")
    else()
      message(FATAL_ERROR "Specified order file '${STONE_ORDER_FILE}' does not exist.")
    endif()
  endif()
endif()


if( STONE_INCLUDE_TESTS )
  find_package(Perl)

  #add_subdirectory(unittests)
  list(APPEND STONE_TEST_DEPS StoneUnitTests)
  list(APPEND STONE_TEST_PARAMS
    stone_unit_site_config=${CMAKE_CURRENT_BINARY_DIR}/test/Unit/lit.site.cfg
  )
  #add_subdirectory(test)
  #add_subdirectory(bindings/python/tests)

  if(STONE_BUILT_STANDALONE)
    umbrella_lit_testsuite_end(check-all)
  endif()
  #add_subdirectory(utils/perf-training)
endif()

option(STONE_INCLUDE_DOCS "Generate build targets for the Stone docs."
  ${LLVM_INCLUDE_DOCS})
if( STONE_INCLUDE_DOCS )
  #add_subdirectory(docs)
endif()

# Custom target to install all stone libraries.
add_custom_target(stone-libraries)
set_target_properties(stone-libraries PROPERTIES FOLDER "Stone/Install")

if(NOT LLVM_ENABLE_IDE)
  add_llvm_install_targets(install-stone-libraries
                           DEPENDS stone-libraries
                           COMPONENT stone-libraries)
endif()

get_property(STONE_LIBS GLOBAL PROPERTY STONE_LIBS)
if(STONE_LIBS)
  list(REMOVE_DUPLICATES STONE_LIBS)
  foreach(lib ${STONE_LIBS})
    add_dependencies(stone-libraries ${lib})
    if(NOT LLVM_ENABLE_IDE)
      add_dependencies(install-stone-libraries install-${lib})
      add_dependencies(install-stone-libraries-stripped install-${lib}-stripped)
    endif()
  endforeach()
endif()

add_subdirectory(cmake/modules)

if(STONE_STAGE)
  message(STATUS "Setting current stone stage to: ${STONE_STAGE}")
endif()

if (STONE_ENABLE_BOOTSTRAP)
  include(ExternalProject)

  add_custom_target(stone-bootstrap-deps DEPENDS stone)

  if(NOT STONE_STAGE)
    set(STONE_STAGE stage1)
  endif()

  string(REGEX MATCH "stage([0-9]*)" MATCHED_STAGE "${STONE_STAGE}")
  if(MATCHED_STAGE)
    if(NOT LLVM_BUILD_INSTRUMENTED)
      math(EXPR STAGE_NUM "${CMAKE_MATCH_1} + 1")
      set(NEXT_STONE_STAGE stage${STAGE_NUM})
    else()
      set(NEXT_STONE_STAGE stage${CMAKE_MATCH_1})
    endif()
  else()
    set(NEXT_STONE_STAGE bootstrap)
  endif()

  if(BOOTSTRAP_LLVM_BUILD_INSTRUMENTED)
    set(NEXT_STONE_STAGE ${NEXT_STONE_STAGE}-instrumented)
  endif()
  message(STATUS "Setting next stone stage to: ${NEXT_STONE_STAGE}")


  set(STAMP_DIR ${CMAKE_CURRENT_BINARY_DIR}/${NEXT_STONE_STAGE}-stamps/)
  set(BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/${NEXT_STONE_STAGE}-bins/)

  if(BOOTSTRAP_LLVM_ENABLE_LLD)
    # adding lld to stone-bootstrap-deps without having it enabled in
    # LLVM_ENABLE_PROJECTS just generates a cryptic error message.
    if (NOT "lld" IN_LIST LLVM_ENABLE_PROJECTS)
      message(FATAL_ERROR "LLD is enabled in the bootstrap build, but lld is not in LLVM_ENABLE_PROJECTS")
    endif()
    add_dependencies(stone-bootstrap-deps lld)
  endif()

  if (WIN32)
    # Build llvm-rc and llvm-mt which are needed by the Windows build.
    add_dependencies(stone-bootstrap-deps llvm-rc)
    if(LLVM_ENABLE_LIBXML2)
      add_dependencies(stone-bootstrap-deps llvm-mt)
    endif()
  endif()

  # If the next stage is LTO we need to depend on LTO and possibly lld or LLVMgold
  if(BOOTSTRAP_LLVM_ENABLE_LTO OR LLVM_ENABLE_LTO AND NOT LLVM_BUILD_INSTRUMENTED)
    if(APPLE)
      add_dependencies(stone-bootstrap-deps LTO)
      # on Darwin we need to set DARWIN_LTO_LIBRARY so that -flto will work
      # using the just-built compiler, and we need to override DYLD_LIBRARY_PATH
      # so that the host object file tools will use the just-built libLTO.
      # However if System Integrity Protection is enabled the DYLD variables
      # will be scrubbed from the environment of any base system commands. This
      # includes /bin/sh, which ninja uses when executing build commands. To
      # work around the envar being filtered away we pass it in as a CMake
      # variable, and have LLVM's CMake append the envar to the archiver calls.
      set(LTO_LIBRARY -DDARWIN_LTO_LIBRARY=${LLVM_SHLIB_OUTPUT_INTDIR}/libLTO.dylib
        -DDYLD_LIBRARY_PATH=${LLVM_LIBRARY_OUTPUT_INTDIR})
    elseif(MSVC)
      add_dependencies(stone-bootstrap-deps llvm-lib)
      set(${STONE_STAGE}_AR -DCMAKE_AR=${LLVM_RUNTIME_OUTPUT_INTDIR}/llvm-lib)
    elseif(NOT WIN32)
      add_dependencies(stone-bootstrap-deps llvm-ar llvm-ranlib)
      if(NOT BOOTSTRAP_LLVM_ENABLE_LLD AND LLVM_BINUTILS_INCDIR)
        add_dependencies(stone-bootstrap-deps LLVMgold)
      endif()
      set(${STONE_STAGE}_AR -DCMAKE_AR=${LLVM_RUNTIME_OUTPUT_INTDIR}/llvm-ar)
      set(${STONE_STAGE}_RANLIB -DCMAKE_RANLIB=${LLVM_RUNTIME_OUTPUT_INTDIR}/llvm-ranlib)
    endif()
  endif()

  if(STONE_BOOTSTRAP_EXTRA_DEPS)
    add_dependencies(stone-bootstrap-deps ${STONE_BOOTSTRAP_EXTRA_DEPS})
  endif()

  add_custom_target(${NEXT_STONE_STAGE}-clear
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${NEXT_STONE_STAGE}-cleared
    )
  add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${NEXT_STONE_STAGE}-cleared
    DEPENDS stone-bootstrap-deps
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${BINARY_DIR}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${BINARY_DIR}
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${STAMP_DIR}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${STAMP_DIR}
    COMMENT "Clobberring ${NEXT_STONE_STAGE} build and stamp directories"
    )

  if(CMAKE_VERBOSE_MAKEFILE)
    set(verbose -DCMAKE_VERBOSE_MAKEFILE=On)
  endif()

  set(_BOOTSTRAP_DEFAULT_PASSTHROUGH
    PACKAGE_VERSION
    PACKAGE_VENDOR
    LLVM_VERSION_MAJOR
    LLVM_VERSION_MINOR
    LLVM_VERSION_PATCH
    STONE_VERSION_MAJOR
    STONE_VERSION_MINOR
    STONE_VERSION_PATCHLEVEL
    STONE_VERSION_SUFFIX
    STONE_VENDOR
    LLVM_VERSION_SUFFIX
    LLVM_BINUTILS_INCDIR
    STONE_REPOSITORY_STRING
    CMAKE_MAKE_PROGRAM
    CMAKE_OSX_ARCHITECTURES
    CMAKE_BUILD_TYPE
    LLVM_ENABLE_PROJECTS
    LLVM_ENABLE_RUNTIMES)

  # We don't need to depend on compiler-rt/libcxx if we're building instrumented
  # because the next stage will use the same compiler used to build this stage.
  if(NOT LLVM_BUILD_INSTRUMENTED)
    if(TARGET compiler-rt)
      add_dependencies(stone-bootstrap-deps compiler-rt)
    endif()
    if(TARGET cxx-headers)
      add_dependencies(stone-bootstrap-deps cxx-headers)
    endif()
  endif()

  set(C_COMPILER "stone")
  set(CXX_COMPILER "stone++")
  if(WIN32)
    set(C_COMPILER "stone-cl.exe")
    set(CXX_COMPILER "stone-cl.exe")
  endif()

  set(COMPILER_OPTIONS
    -DCMAKE_CXX_COMPILER=${LLVM_RUNTIME_OUTPUT_INTDIR}/${CXX_COMPILER}
    -DCMAKE_C_COMPILER=${LLVM_RUNTIME_OUTPUT_INTDIR}/${C_COMPILER}
    -DCMAKE_ASM_COMPILER=${LLVM_RUNTIME_OUTPUT_INTDIR}/${C_COMPILER}
    -DCMAKE_ASM_COMPILER_ID=Stone)

  # cmake requires CMAKE_LINKER to be specified if the compiler is MSVC-like,
  # otherwise it defaults the linker to be link.exe.
  if(BOOTSTRAP_LLVM_ENABLE_LLD)
    if((WIN32 AND NOT BOOTSTRAP_CMAKE_SYSTEM_NAME) OR BOOTSTRAP_CMAKE_SYSTEM_NAME STREQUAL "Windows")
      set(${STONE_STAGE}_LINKER -DCMAKE_LINKER=${LLVM_RUNTIME_OUTPUT_INTDIR}/lld-link${CMAKE_EXECUTABLE_SUFFIX})
    endif()
  endif()

  if(BOOTSTRAP_CMAKE_SYSTEM_NAME)
    set(${STONE_STAGE}_TABLEGEN
      -DLLVM_TABLEGEN=${LLVM_RUNTIME_OUTPUT_INTDIR}/llvm-tblgen
      -DSTONE_TABLEGEN=${LLVM_RUNTIME_OUTPUT_INTDIR}/stone-tblgen)
    if(BOOTSTRAP_CMAKE_SYSTEM_NAME STREQUAL "Linux")
      if(BOOTSTRAP_LLVM_ENABLE_LLD)
        set(${STONE_STAGE}_LINKER -DCMAKE_LINKER=${LLVM_RUNTIME_OUTPUT_INTDIR}/ld.lld)
      endif()
      if(NOT BOOTSTRAP_LLVM_ENABLE_LTO)
        add_dependencies(stone-bootstrap-deps llvm-ar llvm-ranlib)
        set(${STONE_STAGE}_AR -DCMAKE_AR=${LLVM_RUNTIME_OUTPUT_INTDIR}/llvm-ar)
        set(${STONE_STAGE}_RANLIB -DCMAKE_RANLIB=${LLVM_RUNTIME_OUTPUT_INTDIR}/llvm-ranlib)
      endif()
      add_dependencies(stone-bootstrap-deps llvm-objcopy llvm-strip)
      set(${STONE_STAGE}_OBJCOPY -DCMAKE_OBJCOPY=${LLVM_RUNTIME_OUTPUT_INTDIR}/llvm-objcopy)
      set(${STONE_STAGE}_STRIP -DCMAKE_STRIP=${LLVM_RUNTIME_OUTPUT_INTDIR}/llvm-strip)
      set(${STONE_STAGE}_READELF -DCMAKE_READELF=${LLVM_RUNTIME_OUTPUT_INTDIR}/llvm-readelf)
    endif()
  endif()

  if(BOOTSTRAP_LLVM_BUILD_INSTRUMENTED)
    add_dependencies(stone-bootstrap-deps llvm-profdata)
    set(PGO_OPT -DLLVM_PROFDATA=${LLVM_RUNTIME_OUTPUT_INTDIR}/llvm-profdata)
  endif()

  if(LLVM_BUILD_INSTRUMENTED)
    add_dependencies(stone-bootstrap-deps generate-profdata)
    set(PGO_OPT -DLLVM_PROFDATA_FILE=${CMAKE_CURRENT_BINARY_DIR}/utils/perf-training/stone.profdata)
    # Use the current tools for LTO instead of the instrumented ones
    list(APPEND _BOOTSTRAP_DEFAULT_PASSTHROUGH
      CMAKE_CXX_COMPILER
      CMAKE_C_COMPILER
      CMAKE_ASM_COMPILER
      CMAKE_AR
      CMAKE_RANLIB
      DARWIN_LTO_LIBRARY
      DYLD_LIBRARY_PATH)

    set(COMPILER_OPTIONS)
    set(LTO_LIBRARY)
    set(LTO_AR)
    set(LTO_RANLIB)
  endif()

  # Populate the passthrough variables
  foreach(variableName ${STONE_BOOTSTRAP_PASSTHROUGH} ${_BOOTSTRAP_DEFAULT_PASSTHROUGH})
    if(DEFINED ${variableName})
      if("${${variableName}}" STREQUAL "")
        set(value "")
      else()
        string(REPLACE ";" "|" value "${${variableName}}")
      endif()
      list(APPEND PASSTHROUGH_VARIABLES
        -D${variableName}=${value})
    endif()
  endforeach()

  # Find all variables that start with BOOTSTRAP_ and populate a variable with
  # them.
  get_cmake_property(variableNames VARIABLES)
  foreach(variableName ${variableNames})
    if(variableName MATCHES "^BOOTSTRAP_")
      string(SUBSTRING ${variableName} 10 -1 varName)
      string(REPLACE ";" "|" value "${${variableName}}")
      list(APPEND PASSTHROUGH_VARIABLES
        -D${varName}=${value})
    endif()
    if(${variableName} AND variableName MATCHES "LLVM_EXTERNAL_.*_SOURCE_DIR")
      list(APPEND PASSTHROUGH_VARIABLES
        -D${variableName}=${${variableName}})
    endif()
  endforeach()

  # Build arguments for native tool used in CMake.
  set(build_configuration "$<CONFIG>")
  set(build_tool_args "${LLVM_EXTERNAL_PROJECT_BUILD_TOOL_ARGS}")
  if(NOT build_tool_args STREQUAL "")
    string(PREPEND build_tool_args "-- ")
    separate_arguments(build_tool_args UNIX_COMMAND "${build_tool_args}")
  endif()

  ExternalProject_Add(${NEXT_STONE_STAGE}
    DEPENDS stone-bootstrap-deps
    PREFIX ${NEXT_STONE_STAGE}
    SOURCE_DIR ${CMAKE_SOURCE_DIR}
    STAMP_DIR ${STAMP_DIR}
    BINARY_DIR ${BINARY_DIR}
    EXCLUDE_FROM_ALL 1
    CMAKE_ARGS
                # We shouldn't need to set this here, but INSTALL_DIR doesn't
                # seem to work, so instead I'm passing this through
                -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
                ${PASSTHROUGH_VARIABLES}
                ${STONE_BOOTSTRAP_CMAKE_ARGS}
                 -DSTONE_STAGE=${NEXT_STONE_STAGE}
                ${COMPILER_OPTIONS}
                ${${STONE_STAGE}_TABLEGEN}
                ${LTO_LIBRARY} ${verbose} ${PGO_OPT}
                ${${STONE_STAGE}_LINKER}
                ${${STONE_STAGE}_AR}
                ${${STONE_STAGE}_RANLIB}
                ${${STONE_STAGE}_OBJCOPY}
                ${${STONE_STAGE}_STRIP}
    BUILD_COMMAND ${CMAKE_COMMAND} --build ${BINARY_DIR}
                                   --config ${build_configuration}
                                   ${build_tool_args}
    INSTALL_COMMAND ""
    STEP_TARGETS configure build
    USES_TERMINAL_CONFIGURE 1
    USES_TERMINAL_BUILD 1
    USES_TERMINAL_INSTALL 1
    LIST_SEPARATOR |
    )

  # exclude really-install from main target
  set_target_properties(${NEXT_STONE_STAGE} PROPERTIES _EP_really-install_EXCLUDE_FROM_MAIN On)
  ExternalProject_Add_Step(${NEXT_STONE_STAGE} really-install
    COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> --target install
    COMMENT "Performing install step for '${NEXT_STONE_STAGE}'"
    DEPENDEES build
    USES_TERMINAL 1
  )
  ExternalProject_Add_StepTargets(${NEXT_STONE_STAGE} really-install)
  add_custom_target(${NEXT_STONE_STAGE}-install DEPENDS ${NEXT_STONE_STAGE}-really-install)

  if(NOT STONE_BOOTSTRAP_TARGETS)
    set(STONE_BOOTSTRAP_TARGETS check-llvm check-stone check-all)
  endif()
  foreach(target ${STONE_BOOTSTRAP_TARGETS})

    ExternalProject_Add_Step(${NEXT_STONE_STAGE} ${target}
      COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> --target ${target}
      COMMENT "Performing ${target} for '${NEXT_STONE_STAGE}'"
      DEPENDEES configure
      # We need to set ALWAYS to ON here, otherwise these targets won't be
      # built on a second invocation of ninja.  The targets have their own
      # logic to determine if they should build or not so setting ALWAYS ON
      # here does not mean the targets will always rebuild it just means that
      # they will check their dependenices and see if they need to be built.
      ALWAYS ON
      EXCLUDE_FROM_MAIN ON
      USES_TERMINAL 1
    )

    if(target MATCHES "^stage[0-9]*")
      add_custom_target(${target} DEPENDS ${NEXT_STONE_STAGE}-${target})
    endif()

    ExternalProject_Add_StepTargets(${NEXT_STONE_STAGE} ${target})
  endforeach()
endif()

set(STONE_BOLT OFF CACHE STRING "Apply BOLT optimization to Stone. \
  May be specified as Instrument or Perf or LBR to use a particular profiling \
  mechanism.")
string(TOUPPER "${STONE_BOLT}" STONE_BOLT)

if (STONE_BOLT AND NOT LLVM_BUILD_INSTRUMENTED)
  set(STONE_PATH ${LLVM_RUNTIME_OUTPUT_INTDIR}/stone)
  set(STONE_INSTRUMENTED ${LLVM_RUNTIME_OUTPUT_INTDIR}/${STONE_BOLT_INSTRUMENTED})
  set(BOLT_FDATA ${CMAKE_CURRENT_BINARY_DIR}/utils/perf-training/prof.fdata)

  # Pass extra flag in no-LBR mode
  if (STONE_BOLT STREQUAL "PERF")
    set(BOLT_NO_LBR "-nl")
  endif()

  if (STONE_BOLT STREQUAL "INSTRUMENT")
    # Instrument stone with BOLT
    add_custom_target(stone-instrumented
      DEPENDS ${STONE_INSTRUMENTED}
    )
    add_custom_command(OUTPUT ${STONE_INSTRUMENTED}
      DEPENDS stone llvm-bolt
      COMMAND llvm-bolt ${STONE_PATH} -o ${STONE_INSTRUMENTED}
        -instrument --instrumentation-file-append-pid
        --instrumentation-file=${BOLT_FDATA}
      COMMENT "Instrumenting stone binary with BOLT"
      VERBATIM
    )
    add_custom_target(stone-bolt-training-deps DEPENDS stone-instrumented)
  else() # perf or LBR
    add_custom_target(stone-bolt-training-deps DEPENDS stone)
  endif()

  # Optimize original (pre-bolt) Stone using the collected profile
  set(STONE_OPTIMIZED ${CMAKE_CURRENT_BINARY_DIR}/stone.bolt)
  add_custom_target(stone-bolt
    DEPENDS ${STONE_OPTIMIZED}
  )
  add_custom_command(OUTPUT ${STONE_OPTIMIZED}
    DEPENDS stone-bolt-profile
    COMMAND llvm-bolt ${STONE_PATH}
      -o ${STONE_OPTIMIZED}
      -data ${BOLT_FDATA}
      -reorder-blocks=ext-tsp -reorder-functions=hfsort+ -split-functions
      -split-all-cold -split-eh -dyno-stats -icf=1 -use-gnu-stack
      ${BOLT_NO_LBR}
    COMMAND ${CMAKE_COMMAND} -E rename ${STONE_OPTIMIZED} $<TARGET_FILE:stone>
    COMMENT "Optimizing Stone with BOLT"
    VERBATIM
  )
endif()

if (LLVM_ADD_NATIVE_VISUALIZERS_TO_SOLUTION)
  #add_subdirectory(utils/StoneVisualizers)
endif()
#add_subdirectory(utils/hmaptool)

if(STONE_BUILT_STANDALONE)
  llvm_distribution_add_targets()
  process_llvm_pass_plugins()
endif()

set(STONE_INSTALL_LIBDIR_BASENAME "lib${STONE_LIBDIR_SUFFIX}")

configure_file(
  ${STONE_SOURCE_DIR}/include/stone/config.h.cmake
  ${STONE_BINARY_DIR}/include/stone/config.h)
