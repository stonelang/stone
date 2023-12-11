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

		GenCall.cpp
		GenDecl.cpp
		GenExpr.cpp
		GenFun.cpp
		GenIR.cpp
		GenNative.cpp
		GenStmt.cpp
		GenStruct.cpp
		GenBuiltin.cpp
		GenType.cpp
		GenABI.cpp
		IRCodeGenConstant.cpp
		IRCodeGenDebug.cpp
		IRCodeGenBuilder.cpp
		IRCodeGenFunction.cpp
		IRCodegenModule.cpp
		IRCodeGenRequest.cpp
		IRCodeGenMetadata.cpp
		IRCodeGenTypeResolver.cpp
		OptimizeIR.cpp

		LINK_LIBS
		StoneSyntax
		clangCodeGen;
		
)
add_stone_library(StoneGen
	${stone_gen_sources}
)



