set(stone_compile_sources
	Compile.cpp
	CompilerInstance.cpp
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
	StoneAST
	StoneBasic
	StoneCodeAna
	StoneCodeGen
	clangFrontend

	DEPENDS
	StoneOpts
)
add_stone_library(StoneCompile
	${stone_compile_sources}
)



