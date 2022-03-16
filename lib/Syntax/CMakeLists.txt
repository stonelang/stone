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
	Module.cpp
	Stmt.cpp
	Syntax.cpp
	SyntaxContext.cpp
	SyntaxBuilder.cpp
	SyntaxDiagnostic.cpp
	SyntaxScope.cpp
	SyntaxVisitor.cpp
	SyntaxWalker.cpp
	Template.cpp
	Type.cpp
	VerifyDecl.cpp
	Witness.cpp
	
	LINK_LIBS
	StoneCore
)
add_stone_library(StoneSyntax
	${stone_syn_sources}
)


