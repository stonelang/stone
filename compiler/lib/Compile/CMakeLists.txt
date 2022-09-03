set(stone_compile_sources
	Compile.cpp
	CompilerInstance.cpp
	Deserialization.cpp
	CompilerInvocation.cpp
	CompilerOptions.cpp
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
	DEPENDS
	StoneOptions
)
add_stone_library(StoneCompile
	${stone_compile_sources}
)



