set(LLVM_LINK_COMPONENTS
  BinaryFormat
  Core
  Support
 )

set(stone_syn_sources
	Availability.cpp
	Builtin.cpp
	Comment.cpp
	Decl.cpp
	DeclSpecifier.cpp
	DeclContext.cpp
	DeclGroup.cpp
	DeclName.cpp
	Diagnostics.cpp
	DiagnosticList.cpp
	Expr.cpp
	Identifier.cpp
	Module.cpp
	MemberTable.cpp
	NameLookup.cpp
	CreateDecl.cpp
	CreateExpr.cpp
	CreateStmt.cpp
	SearchPath.cpp
	Stmt.cpp
	Substituion.cpp
	ASTContext.cpp
	ASTDiagnostic.cpp
	Scope.cpp
	ASTVisitor.cpp
	ASTWalker.cpp
	Generics.cpp
	Type.cpp
	TypeCollector.cpp
	TypeSpecifier.cpp
	VerifyDecl.cpp
	VirtualTable.cpp

	ClangContext.cpp
	ClangModuleImporter.cpp
	ClangAST.cpp


	LINK_LIBS
	StoneSupport
)
add_stone_library(StoneAST
	${stone_syn_sources}
)


