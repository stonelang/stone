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
set(stone_clang_sources
	ClangModuleImporter.cpp
	ClangNode.cpp

	LINK_LIBS
	StoneBasic
)
add_stone_library(StoneClang
	${stone_clang_sources}
)



