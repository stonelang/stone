set(stone_include_files
	${STONE_SOURCE_DIR}/compiler/include/stone/*.h
	${STONE_SOURCE_DIR}/compiler/include/stone/AST/*.h
	${STONE_SOURCE_DIR}/compiler/include/stone/Parse/*.h
	${STONE_SOURCE_DIR}/compiler/include/stone/Basic/*.h
	${STONE_SOURCE_DIR}/compiler/include/stone/Compile/*.h
	${STONE_SOURCE_DIR}/compiler/include/stone/Diag/*.h
	${STONE_SOURCE_DIR}/compiler/include/stone/Driver/*.h
	${STONE_SOURCE_DIR}/compiler/include/stone/CodeGen/*.h
	${STONE_SOURCE_DIR}/compiler/include/stone/Support/*.h
	${STONE_SOURCE_DIR}/compiler/include/stone/Sem/*.h
	${STONE_SOURCE_DIR}/utils/stone-tblgen/*.h
	
	
)
set(stone_lib_files
	${STONE_SOURCE_DIR}/compiler/lib/AST/*.cpp
	${STONE_SOURCE_DIR}/compiler/lib/Basic/*.cpp
	${STONE_SOURCE_DIR}/compiler/lib/Compile/*.cpp
	${STONE_SOURCE_DIR}/compiler/lib/Diag/*.cpp
	${STONE_SOURCE_DIR}/compiler/lib/Parse/*.cpp
	${STONE_SOURCE_DIR}/compiler/lib/Sem/*.cpp
	${STONE_SOURCE_DIR}/compiler/lib/Driver/*.cpp
	${STONE_SOURCE_DIR}/compiler/lib/CodeGen/*.cpp
	${STONE_SOURCE_DIR}/compiler/lib/Support/*.cpp
	${STONE_SOURCE_DIR}/utils/stone-tblgen/*.cpp
	
)
set(stone_tools_files
	${STONE_SOURCE_DIR}/tools/compile/*.cpp  
	${STONE_SOURCE_DIR}/tools/driver/*.cpp  
)
set(stone_tests_files
	${STONE_SOURCE_DIR}/tests/units/Basic/*.cpp 
	#${STONE_SOURCE_DIR}/tests/units/Gen/*.cpp 
	${STONE_SOURCE_DIR}/tests/units/Compile/*.cpp 
	${STONE_SOURCE_DIR}/tests/units/Drive/*.cpp 
	#${STONE_SOURCE_DIR}/tests/units/Parse/*.cpp 
	#${STONE_SOURCE_DIR}/tests/units/Sem/*.cpp 
	${STONE_SOURCE_DIR}/tests/units/Lex/*.cpp 
	${STONE_SOURCE_DIR}/tests/units/Syntax/*.cpp 
)
find_program(CLANG_FORMAT clang-format)
	if(CLANG_FORMAT)
	add_custom_target(
		stone-format
		clang-format
		-i
		-style=llvm
		${stone_lib_files}
		${stone_include_files}
	)
endif()
