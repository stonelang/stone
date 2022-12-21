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
set(stone_foreign_sources
	Clang.cpp
	ClangModuleImporter.cpp
	ClangNode.cpp

	LINK_LIBS
	StoneBasic
	clangFrontend
)
add_stone_library(StoneForeign
	${stone_foreign_sources}
)



