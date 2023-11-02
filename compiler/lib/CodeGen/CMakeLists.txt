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
	CodeGenConstant.cpp
	CodeGenDebug.cpp
	CodeGenBuilder.cpp
	CodeGenFunction.cpp
	CodegenModule.cpp
	CodeGenMetadata.cpp
	CodeGenTypeResolver.cpp
	GenCall.cpp
	GenDecl.cpp
	GenStmt.cpp
	GenExpr.cpp
	GenIR.cpp
	GenBuiltin.cpp
	GenNative.cpp
	GenType.cpp
	GenABI.cpp
	
	OptimizeIR.cpp
	
	LINK_LIBS
	StoneAST
	StoneOptions
	
)
add_stone_library(StoneCodeGen
	${stone_gen_sources}
)



