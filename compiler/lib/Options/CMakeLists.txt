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
	Mode.cpp
	Options.cpp

	LINK_LIBS 
	StoneBasic
	StoneDiag
)
add_stone_library(StoneSession
	${stone_option_sources}

	DEPENDS
	StoneOptions
)
