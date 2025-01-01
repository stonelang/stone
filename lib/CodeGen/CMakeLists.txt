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
		CodeGenBackend.cpp
		CodeGenContext.cpp
		CodeGenBuilder.cpp  
		CodeGenFunction.cpp
		CodeGenModule.cpp
		CodeGenPassManager.cpp
		GenClass.cpp
		GenDecl.cpp 
		GenExpr.cpp
		GenInterface.cpp
		GenStmt.cpp 
		GenType.cpp
		GenFun.cpp
		GenStruct.cpp

		LINK_LIBS
		StoneAST
		clangCodeGen;
)
add_stone_library(StoneCodeGen
	${stone_codegen_sources}
)


