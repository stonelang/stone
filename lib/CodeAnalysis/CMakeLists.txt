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
set(stone_code_analysis_sources
	TypeCheck.cpp
	CheckDecl.cpp
	TypeChecker.cpp
	CheckExpr.cpp
	CheckStmt.cpp
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
add_stone_library(StoneCodeAnalysis
	${stone_code_analysis_sources}
)



