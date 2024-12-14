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
set(stone_support_sources
  Statistics.cpp
  Options.cpp

	LINK_LIBS 
	StoneBasic

  DEPENDS
  StoneOptions

)
add_stone_library(StoneSupport
	${stone_support_sources}
)
