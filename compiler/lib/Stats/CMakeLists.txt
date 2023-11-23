set(stone_stats_sources
	Stats.cpp

	LINK_LIBS
	StoneBasic

	DEPENDS
	StoneOptions
)
add_stone_library(StoneStats
	${stone_stats_sources}
)



