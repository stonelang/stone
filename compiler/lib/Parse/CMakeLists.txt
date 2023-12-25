set(stone_parse_sources
	Confusable.cpp
	CollectSpecifier.cpp
	Lexer.cpp
	Parse.cpp
	ParseDecl.cpp
	ParseExpr.cpp
	Parser.cpp
	ParseStmt.cpp
	ParseType.cpp
	Lexing.cpp
	Parsing.cpp

	LINK_LIBS
	StoneSyntax

	DEPENDS
	StoneOptions
)
add_stone_library(StoneParse
	${stone_parse_sources}
)



