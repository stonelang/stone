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
set(stone_gen_sources

	GenIR.cpp
	GenNative.cpp
	IRGen.cpp
	IRBlocks.cpp
	IRFunction.cpp
	IRModule.cpp
	NativeGen.cpp
	Optimize.cpp
	CodeGenContext.cpp
	
	LINK_LIBS
	StoneSyntax
)
add_stone_library(StoneGen
	${stone_gen_sources}
)



