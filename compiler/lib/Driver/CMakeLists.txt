 set(stone_drive_sources
	
	DriverExecution.cpp
	DriverInstance.cpp
	DriverInvocation.cpp
	DriverOptions.cpp
	Main.cpp

  LINK_LIBS
  StoneStats
  StoneOption

)
add_stone_library(StoneDriver
	${stone_drive_sources}

	DEPENDS
	StoneOptions
)
