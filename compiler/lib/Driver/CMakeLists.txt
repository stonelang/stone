  set(stone_drive_sources
	
	BuildSystem.cpp
	Command.cpp
	Compilation.cpp
	CompileJob.cpp
	CompilationJob.cpp
	CompilationModel.cpp
	DriverOptions.cpp
	DarwinTaskQueue.cpp
	DarwinToolChain.cpp
	DebugCompilationListener.cpp
	Main.cpp
	Driver.cpp
	DriverUtil.cpp
	Job.cpp
	TaskQueue.cpp
	JobAction.cpp
	LinkJob.cpp
	LinuxTaskQueue.cpp
	LinuxToolChain.cpp
	ToolChain.cpp
	Task.cpp
	UnixTaskQueue.cpp
	UnixToolChain.cpp

	LINK_LIBS 
	StoneSession
)
add_stone_library(StoneDriver
	${stone_drive_sources}

	DEPENDS
	StoneOptions
)
