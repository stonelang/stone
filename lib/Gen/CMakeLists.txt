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
	CodeGenContext.cpp
	GenDecl.cpp
	GenStmt.cpp
	GenExpr.cpp
	GenIR.cpp
	GenNative.cpp
	IRCodeGen.cpp
	IRCodeGenBuilder.cpp
	IRCodeGenFunction.cpp
	IRCodegenModule.cpp
	NativeCodeGen.cpp
	Optimize.cpp
	
	
	LINK_LIBS
	StoneSyntax
)
add_stone_library(StoneGen
	${stone_gen_sources}
)



