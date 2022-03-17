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
	BaseOptions.cpp
	OptInvocation.cpp	
	Options.cpp

	LINK_LIBS 
	StoneCore
)
add_stone_library(StoneOption
	${stone_option_sources}

	DEPENDS
	StoneOptions
)
