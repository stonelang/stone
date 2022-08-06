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
	DeclSyntaxBuilder.cpp
	Expr.cpp
	ExprSyntaxBuilder.cpp
	Identifier.cpp
	Module.cpp
	Stmt.cpp
	StmtSyntaxBuilder.cpp
	Syntax.cpp
	SyntaxContext.cpp
	SyntaxBuilder.cpp
	SyntaxDiagnostic.cpp
	SyntaxFactory.cpp
	SyntaxScope.cpp
	SyntaxVisitor.cpp
	SyntaxWalker.cpp
	Template.cpp
	Type.cpp
	VerifyDecl.cpp
	VirtualTable.cpp
	
	LINK_LIBS
	StoneBasic
	StoneClang
	StoneDiag
)
add_stone_library(StoneSyntax
	${stone_syn_sources}
)


