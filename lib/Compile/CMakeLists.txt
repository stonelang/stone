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
	Compilable.cpp
	CompilableItem.cpp
	Compile.cpp
	Compiler.cpp
	Modes.cpp
	Serialize.cpp	

	LINK_LIBS
	StoneCodeGen
	StoneParse
	StoneSemantics
	StoneSession

	DEPENDS
	StoneOptions
)
add_stone_library(StoneCompile
	${stone_compile_sources}
)



