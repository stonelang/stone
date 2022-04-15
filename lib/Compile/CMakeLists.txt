set(stone_compile_sources
	Compile.cpp
	DebugLangListener.cpp
	LangInstance.cpp
	LangInvocation.cpp
	ModuleSystem.cpp
	ModuleDependencies.cpp
	PackageSystem.cpp
	TargetMachine.cpp
	Workspace.cpp

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



