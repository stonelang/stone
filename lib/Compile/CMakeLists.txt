set(stone_compile_sources
	Compile.cpp
	DebugLangListener.cpp
	JustAnalysis.cpp
	JustCodeGen.cpp
	Lang.cpp
	Frontend.cpp
	Lexer.cpp
	LexerCache.cpp
	ModuleSystem.cpp
	Parse.cpp
	ParseDecl.cpp
	Parser.cpp
	ParseExpr.cpp
	ParseStmt.cpp
	TypeCheck.cpp
	TypeCheckDecl.cpp
	TypeChecker.cpp
	TypeCheckExpr.cpp
	TypeCheckStmt.cpp

	LINK_LIBS
	StoneGen
	StoneSyntax
	StoneOption

	DEPENDS
	StoneOptions
)
add_stone_library(StoneCompile
	${stone_compile_sources}
)



