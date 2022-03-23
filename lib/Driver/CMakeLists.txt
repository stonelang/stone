  set(stone_driver_sources
	
	BuildSystem.cpp
	Command.cpp
	Compilation.cpp
	CompileJob.cpp
	CompilationJob.cpp
	CompilationModel.cpp
	DarwinTaskQueue.cpp
	DarwinToolChain.cpp
	DebugCompilationListener.cpp
	Driver.cpp
	Job.cpp
	TaskQueue.cpp
	Request.cpp
	LinkJob.cpp
	LinuxTaskQueue.cpp
	LinuxToolChain.cpp
	Run.cpp
	ToolChain.cpp
	UnixTaskQueue.cpp
	UnixToolChain.cpp

	LINK_LIBS 
	StoneSession
)
add_stone_library(StoneDriver
	${stone_driver_sources}

	DEPENDS
	StoneOptions
)
