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
set(stone_option_sources
	Action.cpp
	Options.cpp

	LINK_LIBS 
	StoneBasic
	StoneDiag
)
add_stone_library(StoneOption
	${stone_option_sources}

	DEPENDS
	StoneOptions
)
