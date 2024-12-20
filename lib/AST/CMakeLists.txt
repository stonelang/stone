set(LLVM_LINK_COMPONENTS
  BinaryFormat
  Core
  Support
 )

set(stone_ast_sources
	Availability.cpp
	Builtin.cpp
	Comment.cpp
	Decl.cpp
	DeclContext.cpp
	DeclName.cpp
	Diagnostics.cpp
	DiagnosticList.cpp
	Expr.cpp
	Identifier.cpp
	Module.cpp
	MemberTable.cpp
	NameLookup.cpp
	SearchPath.cpp
	Stmt.cpp
	Substituion.cpp
	ASTContext.cpp
	ASTScope.cpp
	ASTVisitor.cpp
	ASTWalker.cpp
	Generics.cpp
	Type.cpp
	VerifyDecl.cpp
	VirtualTable.cpp

	ClangImporter.cpp
	ClangAST.cpp


	LINK_LIBS
	StoneSupport
)
add_stone_library(StoneAST
	${stone_ast_sources}
)


