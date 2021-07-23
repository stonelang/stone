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
set(stone_codegen_sources
	CodeGenBlocks.cpp
	CodeGenerator.cpp
	CodeEmitter.cpp
	CodeGenFunction.cpp
	Gen.cpp
	Native.cpp
	Optimize.cpp
	
	LINK_LIBS
	StoneSyntax
)
add_stone_library(StoneCodeGen
	${stone_codegen_sources}
)



