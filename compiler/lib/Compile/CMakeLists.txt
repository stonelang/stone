set(stone_compile_sources
  Compile.cpp
  Compiler.cpp
  CompilerExecution.cpp
  CompilerInvocation.cpp
  CompilerInputsAndOutputs.cpp
  CompilerInputsConverter.cpp
  CompilerOptionsConverter.cpp
  CompilerOutputsConverter.cpp
  SyntaxAnalysisExecution.cpp
  SupportExecution.cpp
  CodeGenExecution.cpp
  SemanticAnalysisExecution.cpp
  FallbackExecution.cpp
  
  LINK_LIBS
  StoneStats
  StoneGen
  StoneParse
  StoneSem
  StoneOption

  clangFrontend

  DEPENDS
  StoneOptions
)
add_stone_library(StoneCompile
  ${stone_compile_sources}
)



