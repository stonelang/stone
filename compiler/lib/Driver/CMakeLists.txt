 set(stone_drive_sources
	
  Driver.cpp
  Job.cpp
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
