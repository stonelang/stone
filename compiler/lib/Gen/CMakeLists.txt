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
		NativeCodeGen.cpp
		CodeGenContext.cpp
		GenCall.cpp
		GenDecl.cpp
		GenExpr.cpp
		GenFun.cpp
		GenIR.cpp
		GenStmt.cpp
		GenStruct.cpp
		GenBuiltin.cpp
		GenNative.cpp
		GenType.cpp
		GenABI.cpp
		IRCodeGenConstant.cpp
		IRCodeGenDebug.cpp
		IRCodeGenBuilder.cpp
		IRCodeGenFunction.cpp
		IRCodegenModule.cpp
		IRCodeGenMetadata.cpp
		IRCodeGenTypeResolver.cpp
		OptimizeIR.cpp

		LINK_LIBS
		StoneSyntax
)
add_stone_library(StoneGen
	${stone_gen_sources}
)



