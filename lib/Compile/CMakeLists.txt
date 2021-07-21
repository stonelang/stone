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
set(stone_compile_sources
	CompilableItem.cpp
	Compile.cpp
	Compiler.cpp
	Modes.cpp
	Serialize.cpp

	Check.cpp
	CheckDecl.cpp
	Checker.cpp
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
	StoneGen
	StoneSession

	DEPENDS
	StoneOptions
)
add_stone_library(StoneCompile
	${stone_compile_sources}
)



