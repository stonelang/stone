set(stone_compile_sources
	Compile.cpp
	CompilerInstance.cpp
	Deserialization.cpp
	CompilerInvocation.cpp
	CompilerInputsAndOutputs.cpp
	CompilerInputsConverter.cpp
	CompilerOptionsConverter.cpp
	CompilerOutputsConverter.cpp
	ModuleSystem.cpp
	ModuleDependencies.cpp
	Serialization.cpp

	TypeCheck.cpp
	TypeCheckAccess.cpp
	TypeCheckAvailability.cpp
	TypeCheckDecl.cpp
	TypeChecker.cpp
	TypeCheckExpr.cpp
	TypeCheckStmt.cpp
	TypeCheckType.cpp
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
	StoneGen
	StoneParse
	StoneSem
	StoneSession
	clangFrontend


	DEPENDS
	StoneOptions
)
add_stone_library(StoneCompile
	${stone_compile_sources}
)



