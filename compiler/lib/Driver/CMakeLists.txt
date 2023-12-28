 set(stone_drive_sources

  Compilation.cpp
  DarwinToolChain.cpp
  Driver.cpp
  Job.cpp
  LinuxToolChain.cpp
  Main.cpp
  ToolChain.cpp
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
