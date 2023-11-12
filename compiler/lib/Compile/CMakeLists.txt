set(stone_compile_sources
	Compile.cpp
	Compiler.cpp
	Deserialization.cpp
	CompilerConfiguration.cpp
	CompilerInputsAndOutputs.cpp
	CompilerInputsConverter.cpp
	CompilerOptionsConverter.cpp
	CompilerOutputsConverter.cpp
	ModuleSystem.cpp
	ModuleDependencies.cpp
	Serialization.cpp

	LINK_LIBS
	StoneGen
	StoneParse
	StoneSem
	StoneSession
	clangFrontend


	DEPENDS
	StoneOptions
)
add_stone_library(StoneCompile
	${stone_compile_sources}
)



