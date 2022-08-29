include(StoneCore)

add_custom_target(StoneUnitTests)

set_target_properties(StoneUnitTests PROPERTIES FOLDER "tests")

function(add_stone_unittest test_dirname)
	add_unittest(StoneUnitTests ${test_dirname} ${ARGN})
endfunction()


find_program(STONE_CORE_UT ${CMAKE_BINARY_DIR}/tools/stone/tests/units/Core/StoneCoreUnitTests)
	if(STONE_CORE_UT)
	add_custom_target(utcore StoneCoreUnitTests)
endif()

find_program(STONE_CHECK_UT ${CMAKE_BINARY_DIR}/tools/stone/tests/units/Check/StoneCheckUnitTests)
	if(STONE_CHECK_UT)
	add_custom_target(utcheck StoneCheckUnitTests)
endif()

find_program(STONE_COMPILE_UT ${CMAKE_BINARY_DIR}/tools/stone/tests/units/Compile/StoneCompileUnitTests)
	if(STONE_COMPILE_UT)
	add_custom_target(utcompile StoneCompileUnitTests)
endif()


find_program(STONE_DRIVER_UT ${CMAKE_BINARY_DIR}/tools/stone/tests/units/Driver/StoneDriverUnitTests)
	if(STONE_DRIVER_UT)
	add_custom_target(utdriver StoneDriverUnitTests)
endif()

find_program(STONE_GEN_UT ${CMAKE_BINARY_DIR}/tools/stone/tests/units/Gen/StoneGenUnitTests)
	if(STONE_GEN_UT)
	add_custom_target(utgen StoneGenUnitTests)
endif()

find_program(STONE_LEX_UT ${CMAKE_BINARY_DIR}/tools/stone/tests/units/Lex/StoneLexUnitTests)
	if(STONE_LEX_UT)
	add_custom_target(utlex StoneLexUnitTests)
endif()

find_program(STONE_PARSE_UT ${CMAKE_BINARY_DIR}/tools/stone/tests/units/Parse/StoneParseUnitTests)
	if(STONE_PARSE_UT)
	add_custom_target(utparse StoneParseUnitTests)
endif()

find_program(STONE_SYN_UT ${CMAKE_BINARY_DIR}/tools/stone/tests/units/Syntax/StoneSyntaxUnitTests)
	if(STONE_SYN_UT)
	add_custom_target(utsyntax StoneSyntaxUnitTests)
endif()









