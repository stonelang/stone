set(stone_compile_sources
	Compile.cpp
	DebugLangListener.cpp
	LangInputsAndOutputs.cpp
	LangInputsConverter.cpp
	LangInstance.cpp
	LangInvocation.cpp
	LangOptionsConverter.cpp
	LangOutputsConverter.cpp
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



