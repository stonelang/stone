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
set(stone_gen_sources
	Diagnostics.cpp
	LINK_LIBS
	StoneBasic
)
add_stone_library(StoneDiagnostics
	${stone_gen_sources}
)



