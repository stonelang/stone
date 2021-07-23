set( LLVM_LINK_COMPONENTS
  ${LLVM_TARGETS_TO_BUILD}
	BitReader
	BinaryFormat
  BitstreamReader
  Support
  Core
  IPO
  Option
  )
set(stone_static_analysis_sources
	StaticChecker.cpp
		
	LINK_LIBS
		StoneSyntax

	DEPENDS
	StoneOptions
)
add_stone_library(StoneStaticAnalysis
	${stone_static_analysis_sources}
)



