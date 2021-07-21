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
	Scope.cpp
	Syntax.cpp
	SyntaxBuilder.cpp
	SyntaxDiagArgument.cpp
	Template.cpp
	TreeContext.cpp
	Type.cpp
	VerifyDecl.cpp
	Visitor.cpp
	Walker.cpp
	Watcher.cpp
	Witness.cpp
	

	LINK_LIBS
	StoneBasic
)
add_stone_library(StoneSyntax
	${stone_syn_sources}
)


