set(stone_parse_sources
	Confusable.cpp
	CollectDecl.cpp
	Lexer.cpp
	Parse.cpp
	ParseDecl.cpp
	ParseExpr.cpp
	Parser.cpp
	ParseStmt.cpp
	ParseType.cpp
	Parsing.cpp

	LINK_LIBS
	StoneAST

	DEPENDS
	StoneOptions
)
add_stone_library(StoneParse
	${stone_parse_sources}
)



