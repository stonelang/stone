set(stone_parse_sources
  Lexer.cpp
  ParseDecl.cpp
  Parser.cpp
  ParseType.cpp
 
  LINK_LIBS
  StoneAST
  StoneSupport

  DEPENDS
  StoneOptions
)
add_stone_library(StoneParse
  ${stone_parse_sources}
)



