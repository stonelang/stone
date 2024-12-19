set(stone_diag_sources
  DiagnosticEngine.cpp
  DiagnosticClient.cpp
  DiagnosticEmitter.cpp
  TextDiagnosticBuffering.cpp
  TextDiagnosticPrinter.cpp
  TextDiagnosticEmitter.cpp

  LINK_LIBS
  StoneBasic

  DEPENDS
  StoneOptions
)
add_stone_library(StoneDiagnostics
  ${stone_diag_sources}
)



