set(stone_compile_sources
	
	Compile.cpp
	CompilerInstance.cpp
	Compiler.cpp
	CompilerCommandLine.cpp
	Deserialization.cpp
	CompilerInvocation.cpp
	CompilerInputsAndOutputs.cpp
	CompilerInputsConverter.cpp
	CompilerOptionsConverter.cpp
	CompilerOutputsConverter.cpp
	ModuleSystem.cpp
	ModuleDependencies.cpp
	Serialization.cpp

	LINK_LIBS
	StoneAnalysis
	StoneAST
	StoneBasic
	StoneOptions
	StoneCodeGen
	clangFrontend

	DEPENDS
	StoneOpts
)
add_stone_library(StoneCompile
	${stone_compile_sources}
)



