set(stone_parse_sources
	Confusable.cpp
	Lexer.cpp
	LexerCache.cpp
	Parse.cpp
	ParseDecl.cpp
	ParseExpr.cpp
	Parser.cpp
	ParseStmt.cpp
	SyntaxParsing.cpp

	LINK_LIBS
	StoneSyntax

	DEPENDS
	StoneOptions
)
add_stone_library(StoneParse
	${stone_parse_sources}
)



