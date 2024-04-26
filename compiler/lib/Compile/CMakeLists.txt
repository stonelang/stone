set(stone_compile_sources
  Compile.cpp
  Compiler.cpp
  CompilerExecution.cpp
  CompilerInvocation.cpp
  CompilerOptions.cpp
  CompilerInputsAndOutputs.cpp
  CompilerInputsConverter.cpp
  CompilerOptionsConverter.cpp
  CompilerOutputsConverter.cpp
  
  LINK_LIBS
  StoneSyntax
  StoneGen
  StoneParse
  StoneSem
  StoneSupport

  clangFrontend

  DEPENDS
  StoneOptions
)
add_stone_library(StoneCompile
  ${stone_compile_sources}
)



