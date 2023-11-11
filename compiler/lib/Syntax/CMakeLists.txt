set(LLVM_LINK_COMPONENTS
  BinaryFormat
  Core
  Support
 )

set(stone_syn_sources
	Availability.cpp
	BuiltinContext.cpp
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
	SyntaxContext.cpp
	SyntaxDiagnostic.cpp
	Scope.cpp
	SyntaxVisitor.cpp
	SyntaxWalker.cpp
	Generics.cpp
	Type.cpp
	TypeQualifier.cpp
	TypeThunk.cpp
	TypeOperator.cpp
	VerifyDecl.cpp
	VirtualTable.cpp

	ClangContext.cpp
	ClangModuleImporter.cpp
	ClangAST.cpp

	
	LINK_LIBS
	StoneDiag
	StoneForeign
)
add_stone_library(StoneSyntax
	${stone_syn_sources}
)


