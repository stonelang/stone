set(stone_sem_sources
	TypeCheck.cpp
	TypeCheckAccess.cpp
	TypeCheckAvailability.cpp
	TypeCheckDecl.cpp
	TypeChecker.cpp
	TypeCheckExpr.cpp
	TypeCheckStmt.cpp
	TypeCheckType.cpp
	ImportResolution.cpp
	
	LINK_LIBS
	StoneSyntax

	DEPENDS
	StoneOptions
)
add_stone_library(StoneSem
	${stone_sem_sources}
)



