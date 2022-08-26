set(LLVM_LINK_COMPONENTS
  BinaryFormat
  Core
  Support
 )

set(stone_syn_sources
	Builtin.cpp
	Comment.cpp
	Decl.cpp
	DeclContext.cpp
	DeclGroup.cpp
	DeclName.cpp
	Expr.cpp
	Identifier.cpp
	MakeDecl.cpp
	MakeExpr.cpp
	MakeStmt.cpp
	Module.cpp
	SearchPath.cpp
	Stmt.cpp
	Substituion.cpp
	Syntax.cpp
	SyntaxContext.cpp
	SyntaxDiagnostic.cpp
	SyntaxScope.cpp
	SyntaxVisitor.cpp
	SyntaxWalker.cpp
	Template.cpp
	Type.cpp
	VerifyDecl.cpp
	VirtualTable.cpp
	
	LINK_LIBS
	StoneBasic
	StoneForeign
	StoneDiag
)
add_stone_library(StoneSyntax
	${stone_syn_sources}
)


