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
set(stone_semantics_sources
	Semantics.cpp
	TypeCheck.cpp
	TypeCheckDecl.cpp
	TypeChecker.cpp
	TypeCheckExpr.cpp
	TypeCheckStmt.cpp

	LINK_LIBS
	StoneSyntax

	DEPENDS
	StoneOptions
)
add_stone_library(StoneSemantics
	${stone_semantics_sources}
)



