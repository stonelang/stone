 set(stone_drive_sources
  Compilation.cpp
  DarwinToolChain.cpp
  Driver.cpp
  DriverOptions.cpp
  DriverInvocation.cpp
  Job.cpp
  JobConstruction.cpp
  LinuxToolChain.cpp
  Main.cpp
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
