set(stone_compile_sources
  Compile.cpp
  CompilerInstance.cpp
  CompilerInvocation.cpp
  CompilerOptions.cpp
  CompilerInputsAndOutputs.cpp
  CompilerInputsConverter.cpp
  CompilerOptionsConverter.cpp
  CompilerOutputsConverter.cpp
  FormatDiagnosticText.cpp
  PerformEmitCode.cpp
  PerformParseSourceFile.cpp
  PerformTypeChecking.cpp
  PerformTypeResolution.cpp

  LINK_LIBS
  StoneCodeGen
  StoneParse
  StoneSupport
  clangFrontend

  DEPENDS
  StoneOptions
)
add_stone_library(StoneCompile
  ${stone_compile_sources}
)



