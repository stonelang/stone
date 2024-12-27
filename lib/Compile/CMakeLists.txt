set(stone_compile_sources
  Compile.cpp
  CompilerInstance.cpp
  CompilerInvocation.cpp
  CompilerOptions.cpp
  CompilerAction.cpp
  CompilerInputsAndOutputs.cpp
  CompilerInputsConverter.cpp
  CompilerOptionsConverter.cpp
  CompilerOutputsConverter.cpp
  FormatDiagnosticText.cpp
  
  LINK_LIBS
  StoneCodeGen
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



