set(stone_compile_sources
	Compile.cpp
	Compiler.cpp
	CompilerCommandLine.cpp
	Deserialization.cpp
	CompilerConfiguration.cpp
	CompilerInputsAndOutputs.cpp
	CompilerInputsConverter.cpp
	CompilerOptionsConverter.cpp
	CompilerOutputsConverter.cpp
	CompilerQueue.cpp
	ModuleSystem.cpp
	ModuleDependencies.cpp
	Serialization.cpp

	LINK_LIBS
	StoneGen
	StoneParse
	StoneSem
	StoneOption
	clangFrontend


	DEPENDS
	StoneOptions
)
add_stone_library(StoneCompile
	${stone_compile_sources}
)



