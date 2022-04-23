set(stone_compile_sources
	Compile.cpp
	DebugFrontendListener.cpp
	FrontendInputsAndOutputs.cpp
	FrontendInputsConverter.cpp
	FrontendInstance.cpp
	FrontendInvocation.cpp
	FrontendOptions.cpp
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



