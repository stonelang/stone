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
set(stone_parse_sources
	Lexer.cpp
	LexerCache.cpp
	Parse.cpp
	ParseDecl.cpp
	Parser.cpp
	ParseExpr.cpp
	ParseStmt.cpp
	
	LINK_LIBS
	StoneSyntax

	DEPENDS
	StoneOptions
)
add_stone_library(StoneParse
	${stone_parse_sources}
)



