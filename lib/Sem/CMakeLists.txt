set(stone_sem_sources
	TypeCheck.cpp
	TypeCheckDecl.cpp
	TypeChecker.cpp
	TypeCheckExpr.cpp
	TypeCheckStmt.cpp
	UsingResolution.cpp
	
	LINK_LIBS
	StoneSyntax

	DEPENDS
	StoneOptions
)
add_stone_library(StoneSem
	${stone_sem_sources}
)



