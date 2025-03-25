set(stone_parse_sources
  
  Confusable.cpp
  Lexer.cpp
  ParseDecl.cpp
  Parser.cpp
  ParseType.cpp
  Parsing.cpp
 
  LINK_LIBS
  StoneAST
  StoneSupport

  DEPENDS
  StoneOptions
)
add_stone_library(StoneParse
  ${stone_parse_sources}
)



