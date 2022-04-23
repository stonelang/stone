set(stone_compile_sources
	Compile.cpp
	DebugFrontendListener.cpp
	Frontend.cpp
	FrontendArguments.cpp
	FrontendInputsAndOutputs.cpp
	FrontendInputsConverter.cpp
	FrontendOptionsConverter.cpp
	FrontendOutputsConverter.cpp
	ModuleSystem.cpp
	ModuleDependencies.cpp
	PackageSystem.cpp
	TargetMachine.cpp

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



