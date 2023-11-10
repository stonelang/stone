set(stone_analysis_sources
	
	TypeCheck.cpp
	TypeCheckAccess.cpp
	TypeCheckAvailability.cpp
	CheckDecl.cpp
	TypeChecker.cpp
	CheckExpr.cpp
	CheckStmt.cpp
	CheckType.cpp
	ImportResolution.cpp

	Confusable.cpp
	CollectDecl.cpp
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
	StoneAST
	StoneBasic
	StoneOptions
	
	DEPENDS
	StoneOpts
)
add_stone_library(StoneAnalysis
	${stone_analysis_sources}
)



