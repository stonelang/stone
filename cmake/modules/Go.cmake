
#set(GOPATH "${CMAKE_CURRENT_BINARY_DIR}/go")

set(GOPATH "${CMAKE_BINARY_DIR}/bin")

#file(MAKE_DIRECTORY ${GOPATH})

function(add_go_tool NAME MAIN_SRC)
  get_filename_component(MAIN_SRC_ABS ${MAIN_SRC} ABSOLUTE)

  add_custom_target(${NAME})

  add_custom_command(TARGET ${NAME}
                    COMMAND env GOPATH=${GOPATH} go build 
                    -o "${CMAKE_CURRENT_BINARY_DIR}/${NAME}"
                    ${CMAKE_GO_FLAGS} ${MAIN_SRC}
                    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
                    DEPENDS ${MAIN_SRC_ABS})

  foreach(DEP ${ARGN})
    add_dependencies(${NAME} ${DEP})
  endforeach()
  
  add_custom_target(${NAME}_all ALL DEPENDS ${NAME})

  install(TARGETS ${CMAKE_CURRENT_BINARY_DIR}/${NAME} RUNTIME DESTINATION bin COMPONENT ${CMAKE_CURRENT_BINARY_DIR}/${NAME})

endfunction(add_go_tool)

set(GOPATH "${CMAKE_BINARY_DIR}/bin")
macro(add_go_tool name)
  if (NOT STONE_BUILD_TOOLS)
    set(EXCLUDE_FROM_ALL ON)
  endif()

  if (STONE_BUILD_TOOLS)
    if(${name} IN_LIST LLVM_DISTRIBUTION_COMPONENTS OR
        NOT LLVM_DISTRIBUTION_COMPONENTS)
      set(export_to_stonetargets EXPORT StoneTargets)
      set_property(GLOBAL PROPERTY STONE_HAS_EXPORTS True)
    endif()



 if(NOT CMAKE_CONFIGURATION_TYPES)
      add_custom_target(install-${name}
        DEPENDS ${name}
        COMMAND "${CMAKE_COMMAND}"
        -DCMAKE_INSTALL_COMPONENT=${name}
        -P "${CMAKE_BINARY_DIR}/cmake_install.cmake")
    endif()

  

add_custom_command(PROGRAM ${name}
                    COMMAND env GOPATH=${GOPATH} go build 
                    -o "${CMAKE_CURRENT_BINARY_DIR}/${name}"
                    ${CMAKE_GO_FLAGS} ${MAIN_SRC}
                    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
                    DEPENDS ${MAIN_SRC_ABS})
    

   install(TARGETS ${name}
      ${export_to_stonetargets}
      RUNTIME DESTINATION bin
      COMPONENT ${name})

    set_property(GLOBAL APPEND PROPERTY STONE_EXPORTS ${name})
  endif()
endmacro()



