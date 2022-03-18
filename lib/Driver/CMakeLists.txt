  set(stone_driver_sources
	
	BuildSystem.cpp
	Compilation.cpp
	CompileJob.cpp
	CompilationJob.cpp
	Command.cpp
	Darwin.cpp
	DebugCompilationListener.cpp
	Driver.cpp
	Intent.cpp
	Job.cpp
	JobQueue.cpp
	JobRequest.cpp
	LinkJob.cpp
	Linux.cpp
	Run.cpp
	ToolChain.cpp
	Unix.cpp

	LINK_LIBS 
	StoneSession
)
add_stone_library(StoneDriver
	${stone_driver_sources}

	DEPENDS
	StoneOptions
)
