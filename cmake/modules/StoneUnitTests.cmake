include(StoneCore)

add_custom_target(StoneUnitTests)

set_target_properties(StoneUnitTests PROPERTIES FOLDER "Tests")

function(add_stone_unittest test_dirname)
	add_unittest(StoneUnitTests ${test_dirname} ${ARGN})
endfunction()


find_program(STONE_CORE_UT ${PATH_TO_LLVM_BUILD}/tools/stone/tests/units/Core/StoneCoreUnitTests)
	if(STONE_CORE_UT)
	add_custom_target(StoneCoreUT StoneCoreUnitTests)
endif()

find_program(STONE_CHECK_UT ${PATH_TO_LLVM_BUILD}/tools/stone/tests/units/Check/StoneCheckUnitTests)
	if(STONE_CHECK_UT)
	add_custom_target(StoneCheckUT StoneCheckUnitTests)
endif()

find_program(STONE_COMPILE_UT ${PATH_TO_LLVM_BUILD}/tools/stone/tests/units/Compile/StoneCompileUnitTests)
	if(STONE_COMPILE_UT)
	add_custom_target(StoneCompileUT StoneCompileUnitTests)
endif()

find_program(STONE_DRIVER_UT ${PATH_TO_LLVM_BUILD}/tools/stone/tests/units/Driver/StoneDriverUnitTests)
	if(STONE_DRIVER_UT)
	add_custom_target(StoneDriverUT StoneDriverUnitTests)
endif()


find_program(STONE_GEN_UT ${PATH_TO_LLVM_BUILD}/tools/stone/tests/units/Gen/StoneGenUnitTests)
	if(STONE_GEN_UT)
	add_custom_target(StoneGenUT StoneGenUnitTests)
endif()

find_program(STONE_LEX_UT ${PATH_TO_LLVM_BUILD}/tools/stone/tests/units/Lex/StoneLexUnitTests)
	if(STONE_LEX_UT)
	add_custom_target(StoneLexUT StoneLexUnitTests)
endif()

find_program(STONE_PARSE_UT ${PATH_TO_LLVM_BUILD}/tools/stone/tests/units/Parse/StoneParseUnitTests)
	if(STONE_PARSE_UT)
	add_custom_target(StoneParseUT StoneParseUnitTests)
endif()

find_program(STONE_SYN_UT ${PATH_TO_LLVM_BUILD}/tools/stone/tests/units/Syntax/StoneSyntaxUnitTests)
	if(STONE_SYN_UT)
	add_custom_target(StoneSyntaxUT StoneSyntaxUnitTests)
endif()









