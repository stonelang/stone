set(stone_sem_sources
  
  TypeCheckDecl.cpp
  TypeChecker.cpp
  
  LINK_LIBS
  StoneAST
  StoneSupport
)
add_stone_library(StoneSem
  ${stone_sem_sources}
)



