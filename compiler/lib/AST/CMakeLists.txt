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
	DeclCollector.cpp
	DeclContext.cpp
	DeclGroup.cpp
	DeclName.cpp
	Expr.cpp
	Identifier.cpp
	Module.cpp
	MemberTable.cpp
	NameLookup.cpp
	SearchPath.cpp
	Specifier.cpp
	Stmt.cpp
	Substituion.cpp
	ASTContext.cpp
	ASTDiagnostic.cpp
	ASTScope.cpp
	ASTVisitor.cpp
	ASTWalker.cpp
	Generics.cpp
	Type.cpp
	TypeQualifier.cpp
	TypeChunk.cpp
	TypeOperator.cpp
	VirtualTable.cpp

	Clang.cpp
	ClangModuleImporter.cpp
	ClangAST.cpp
	
	LINK_LIBS
	StoneDiag

)
add_stone_library(StoneAST
	${stone_ast_sources}
)


