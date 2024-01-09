 set(stone_drive_sources
  Compilation.cpp
  CompilationEntity.cpp
  DarwinToolChain.cpp
  Driver.cpp
  DriverOptions.cpp
  Job.cpp
  JobConstruction.cpp
  LinuxToolChain.cpp
  Main.cpp
  TaskQueue.cpp
  ToolChain.cpp
  UnixToolChain.cpp
  WindowsToolChain.cpp

  LINK_LIBS
  StoneStats
  StoneOption
)
add_stone_library(StoneDriver
	${stone_drive_sources}

	DEPENDS
	StoneOptions
)
