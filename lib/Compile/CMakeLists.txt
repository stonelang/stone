set( LLVM_LINK_COMPONENTS
  ${LLVM_TARGETS_TO_BUILD}
	BitReader
	BinaryFormat
  BitstreamReader
  Support
  Core
  IPO
  Option
  )
set(stone_compile_sources
	CompilableItem.cpp
	Compile.cpp
	Compiler.cpp
	Modes.cpp
	Serialize.cpp	
	
	LINK_LIBS
	StoneCodeAnalysis
	StoneCodeGen
	StoneStaticAnalysis
	StoneSession

	DEPENDS
	StoneOptions
)
add_stone_library(StoneCompile
	${stone_compile_sources}
)



