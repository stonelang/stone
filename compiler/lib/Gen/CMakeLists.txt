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
		IRGenConstant.cpp
		IRGenDebug.cpp
		IRGenBuilder.cpp
		IRGenFunction.cpp
		IRGenInstance.cpp
		IRGenRequest.cpp
		IRGenMetadata.cpp
		IRGenTypeResolver.cpp
		IRGenVirtualTable.cpp
		OptimizeIR.cpp

		LINK_LIBS
		StoneAST
		clangCodeGen;
)
add_stone_library(StoneGen
	${stone_gen_sources}
)



