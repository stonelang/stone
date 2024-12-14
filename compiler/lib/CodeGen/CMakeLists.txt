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
		CodeGen.cpp
		CodeGenDecl.cpp 
		CodeGenExpr.cpp
		CodeGenStmt.cpp 
		CodeGenType.cpp
		CodeGenBuilder.cpp  
		CodeGenFunction.cpp
		CodeGenModule.cpp
		CodeGenPassManager.cpp


		LINK_LIBS
		StoneAST
		clangCodeGen;
)
add_stone_library(StoneCodeGen
	${stone_codegen_sources}
)


