set(stone_compile_sources
	Compile.cpp
	Confusable.cpp
	DebugLangListener.cpp
	LangInstance.cpp
	LangInvocation.cpp
	Lexer.cpp
	LexerCache.cpp
	ModuleSystem.cpp
	Parse.cpp
	ParseDecl.cpp
	Parser.cpp
	ParseExpr.cpp
	ParseStmt.cpp
	SyntaxParsing.cpp
	PerformAnalysis.cpp
	PerformCodeGen.cpp
	TypeCheck.cpp
	TypeCheckDecl.cpp
	TypeChecker.cpp
	TypeCheckExpr.cpp
	TypeCheckStmt.cpp

	LINK_LIBS
	StoneGen
	StoneSession
	StoneSyntax

	DEPENDS
	StoneOptions
)
add_stone_library(StoneCompile
	${stone_compile_sources}
)



