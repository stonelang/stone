set(stone_diag_sources
  DiagnosticEngine.cpp
  DiagnosticID.cpp
  DiagnosticClient.cpp
  DiagnosticEmitter.cpp
  TextDiagnosticBuffering.cpp
  TextDiagnosticPrinter.cpp
  TextDiagnosticEmitter.cpp

  LINK_LIBS
  StoneBasic
  StoneSupport

  DEPENDS
   omp_gen
  StoneOptions

)
add_stone_library(StoneDiag
  ${stone_diag_sources}
)



