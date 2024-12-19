include(TableGen)

# This needs to be a macro since tablegen (which is a function) needs to set
# variables in its parent scope.
#macro(stone_tablegen)
#  tablegen(LLVM ${ARGN})
#endmacro()

# This needs to be a macro since add_public_tablegen_target (which is a
# function) needs to set variables in its parent scope.
#macro(stone_add_public_tablegen_target target)
#  add_public_tablegen_target(${target})
#endmacro()


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
