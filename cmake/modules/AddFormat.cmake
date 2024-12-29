set(stone_include_files
	${STONE_SOURCE_DIR}/include/stone/*.h
	${STONE_SOURCE_DIR}/include/stone/AST/*.h
	${STONE_SOURCE_DIR}/include/stone/Basic/*.h
	${STONE_SOURCE_DIR}/include/stone/Compile/*.h
	${STONE_SOURCE_DIR}/include/stone/CodeGen/*.h
	${STONE_SOURCE_DIR}/include/stone/Driver/*.h
	${STONE_SOURCE_DIR}/include/stone/Parse/*.h
	${STONE_SOURCE_DIR}/include/stone/Support/*.h
	
)
set(stone_lib_files
	${STONE_SOURCE_DIR}/lib/AST/*.cpp
	${STONE_SOURCE_DIR}/lib/Basic/*.cpp
	${STONE_SOURCE_DIR}/lib/Compile/*.cpp
	${STONE_SOURCE_DIR}/lib/Parse/*.cpp
	${STONE_SOURCE_DIR}/lib/Driver/*.cpp
	${STONE_SOURCE_DIR}/lib/CodeGen/*.cpp
	${STONE_SOURCE_DIR}/lib/Support/*.cpp
	
)
set(stone_tools_files
	${STONE_SOURCE_DIR}/tools/compile/*.cpp  
	${STONE_SOURCE_DIR}/tools/driver/*.cpp  
)

set(stone_tests_files
	${STONE_SOURCE_DIR}/tests/units/AST/*.cpp 
	${STONE_SOURCE_DIR}/tests/units/Basic/*.cpp 
	${STONE_SOURCE_DIR}/tests/units/CodeGen/*.cpp 
	${STONE_SOURCE_DIR}/tests/units/Compile/*.cpp 
	${STONE_SOURCE_DIR}/tests/units/Driver/*.cpp 
	${STONE_SOURCE_DIR}/tests/units/Parse/*.cpp 
	
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
		${stone_tools_files}
	)
endif()
