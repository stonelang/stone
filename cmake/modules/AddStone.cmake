include(GNUInstallDirs)
include(LLVMDistributionSupport)

function(stone_tablegen)
  # Syntax:
  # stone_tablegen output-file [tablegen-arg ...] SOURCE source-file
  # [[TARGET cmake-target-name] [DEPENDS extra-dependency ...]]
  #
  # Generates a custom command for invoking tblgen as
  #
  # tblgen source-file -o=output-file tablegen-arg ...
  #
  # and, if cmake-target-name is provided, creates a custom target for
  # executing the custom command depending on output-file. It is
  # possible to list more files to depend after DEPENDS.

  cmake_parse_arguments(CTG "" "SOURCE;TARGET" "" ${ARGN})

  if( NOT CTG_SOURCE )
    message(FATAL_ERROR "SOURCE source-file required by stone_tablegen")
  endif()

  set( STONE_TABLEGEN_ARGUMENTS "" )
  set( LLVM_TARGET_DEFINITIONS ${CTG_SOURCE} )
  tablegen(STONE ${CTG_UNPARSED_ARGUMENTS} ${STONE_TABLEGEN_ARGUMENTS})

  if(CTG_TARGET)
    add_public_tablegen_target(${CTG_TARGET})
    set_property(GLOBAL APPEND PROPERTY STONE_TABLEGEN_TARGETS ${CTG_TARGET})
  endif()
endfunction(stone_tablegen)

macro(set_stone_windows_version_resource_properties name)
  if(DEFINED windows_resource_file)
    set_windows_version_resource_properties(${name} ${windows_resource_file}
      VERSION_MAJOR ${STONE_VERSION_MAJOR}
      VERSION_MINOR ${STONE_VERSION_MINOR}
      VERSION_PATCHLEVEL ${STONE_VERSION_PATCHLEVEL}
      VERSION_STRING "${STONE_VERSION}"
      PRODUCT_NAME "stone")
  endif()
endmacro()

macro(add_stone_subdirectory name)
  add_llvm_subdirectory(STONE TOOL ${name})
endmacro()

macro(add_stone_library name)
  cmake_parse_arguments(ARG "SHARED" "STATIC" "ADDITIONAL_HEADERS" ${ARGN})
  
  set(srcs)
  if(MSVC_IDE OR XCODE)
    # Add public headers
    file(RELATIVE_PATH lib_path
      ${STONE_SOURCE_DIR}/lib/
      ${CMAKE_CURRENT_SOURCE_DIR}
    )
    if(NOT lib_path MATCHES "^[.][.]")
      file( GLOB_RECURSE headers
        ${STONE_SOURCE_DIR}/include/stone/${lib_path}/*.h
        ${STONE_SOURCE_DIR}/include/stone/${lib_path}/*.def
      )
      set_source_files_properties(${headers} PROPERTIES HEADER_FILE_ONLY ON)

      file( GLOB_RECURSE tds
        ${STONE_SOURCE_DIR}/include/stone/${lib_path}/*.td
      )
      source_group("TableGen descriptions" FILES ${tds})
      set_source_files_properties(${tds} PROPERTIES HEADER_FILE_ONLY ON)

      if(headers OR tds)
        set(srcs ${headers} ${tds})
      endif()
    endif()
  endif(MSVC_IDE OR XCODE)
  if(srcs OR ARG_ADDITIONAL_HEADERS)
    set(srcs
      ADDITIONAL_HEADERS
      ${srcs}
      ${ARG_ADDITIONAL_HEADERS} # It may contain unparsed unknown args.
      )
  endif()

  if(ARG_SHARED AND ARG_STATIC)
    set(LIBTYPE SHARED STATIC)
  elseif(ARG_SHARED)
    set(LIBTYPE SHARED)
  else()
    # llvm_add_library ignores BUILD_SHARED_LIBS if STATIC is explicitly set,
    # so we need to handle it here.
    if(BUILD_SHARED_LIBS)
      set(LIBTYPE SHARED)
    else()
      set(LIBTYPE STATIC)
    endif()
    if(NOT XCODE AND NOT MSVC_IDE)
      # The Xcode generator doesn't handle object libraries correctly.
      # The Visual Studio CMake generator does handle object libraries
      # correctly, but it is preferable to list the libraries with their
      # source files (instead of the object files and the source files in
      # a separate target in the "Object Libraries" folder)
      list(APPEND LIBTYPE OBJECT)
    endif()
    set_property(GLOBAL APPEND PROPERTY STONE_STATIC_LIBS ${name})
  endif()
  llvm_add_library(${name} ${LIBTYPE} ${ARG_UNPARSED_ARGUMENTS} ${srcs})

  set(libs ${name})
  if(ARG_SHARED AND ARG_STATIC)
    list(APPEND libs ${name}_static)
  endif()

  foreach(lib ${libs})
    if(TARGET ${lib})
      target_link_libraries(${lib} INTERFACE ${LLVM_COMMON_LIBS})

      if (NOT LLVM_INSTALL_TOOLCHAIN_ONLY OR ARG_INSTALL_WITH_TOOLCHAIN)
        get_target_export_arg(${name} Stone export_to_stonetargets UMBRELLA stone-libraries)
        install(TARGETS ${lib}
          COMPONENT ${lib}
          ${export_to_stonetargets}
          LIBRARY DESTINATION lib${LLVM_LIBDIR_SUFFIX}
          ARCHIVE DESTINATION lib${LLVM_LIBDIR_SUFFIX}
          RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")

        if (NOT LLVM_ENABLE_IDE)
          add_llvm_install_targets(install-${lib}
                                   DEPENDS ${lib}
                                   COMPONENT ${lib})
        endif()

        set_property(GLOBAL APPEND PROPERTY STONE_LIBS ${lib})
      endif()
      set_property(GLOBAL APPEND PROPERTY STONE_EXPORTS ${lib})
    else()
      # Add empty "phony" target
      add_custom_target(${lib})
    endif()
  endforeach()

  set_stone_windows_version_resource_properties(${name})
endmacro(add_stone_library)

macro(add_stone_executable name)
  add_llvm_executable( ${name} ${ARGN} )
  set_stone_windows_version_resource_properties(${name})
endmacro(add_stone_executable)

macro(add_stone_tool name)
  cmake_parse_arguments(ARG "DEPENDS;GENERATE_DRIVER" "" "" ${ARGN})
  if (NOT STONE_BUILD_TOOLS)
    set(EXCLUDE_FROM_ALL ON)
  endif()
  if(ARG_GENERATE_DRIVER
     AND LLVM_TOOL_LLVM_DRIVER_BUILD
     AND (NOT LLVM_DISTRIBUTION_COMPONENTS OR ${name} IN_LIST LLVM_DISTRIBUTION_COMPONENTS)
    )
    set(get_obj_args ${ARGN})
    list(FILTER get_obj_args EXCLUDE REGEX "^SUPPORT_PLUGINS$")
    generate_llvm_objects(${name} ${get_obj_args})
    add_custom_target(${name} DEPENDS llvm-driver stone-resource-headers)
  else()
    add_stone_executable(${name} ${ARGN})
    #add_dependencies(${name} stone-resource-headers)

    if (STONE_BUILD_TOOLS)
      get_target_export_arg(${name} Stone export_to_stonetargets)
      install(TARGETS ${name}
        ${export_to_stonetargets}
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
        COMPONENT ${name})

      if(NOT LLVM_ENABLE_IDE)
        add_llvm_install_targets(install-${name}
                                 DEPENDS ${name}
                                 COMPONENT ${name})
      endif()
      set_property(GLOBAL APPEND PROPERTY STONE_EXPORTS ${name})
    endif()
  endif()
endmacro()

macro(add_stone_symlink name dest)
  get_property(LLVM_DRIVER_TOOLS GLOBAL PROPERTY LLVM_DRIVER_TOOLS)
  if(LLVM_TOOL_LLVM_DRIVER_BUILD
     AND ${dest} IN_LIST LLVM_DRIVER_TOOLS
     AND (NOT LLVM_DISTRIBUTION_COMPONENTS OR ${dest} IN_LIST LLVM_DISTRIBUTION_COMPONENTS)
    )
    set_property(GLOBAL APPEND PROPERTY LLVM_DRIVER_TOOL_ALIASES_${dest} ${name})
  else()
    llvm_add_tool_symlink(STONE ${name} ${dest} ALWAYS_GENERATE)
    # Always generate install targets
    llvm_install_symlink(STONE ${name} ${dest} ALWAYS_GENERATE)
  endif()
endmacro()

function(stone_target_link_libraries target type)
  if (TARGET obj.${target})
    target_link_libraries(obj.${target} ${ARGN})
  endif()

  get_property(LLVM_DRIVER_TOOLS GLOBAL PROPERTY LLVM_DRIVER_TOOLS)
  if(LLVM_TOOL_LLVM_DRIVER_BUILD AND ${target} IN_LIST LLVM_DRIVER_TOOLS)
    set(target llvm-driver)
  endif()

  if (STONE_LINK_STONE_DYLIB)
    target_link_libraries(${target} ${type} stone-cpp)
  else()
    target_link_libraries(${target} ${type} ${ARGN})
  endif()
endfunction()
