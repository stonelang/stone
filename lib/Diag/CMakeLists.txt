set(LLVM_LINK_COMPONENTS
  BinaryFormat
  Core
  FrontendOpenMP
  Support
  TargetParser
  )
set(stone_diag_sources
  DiagnosticEngine.cpp
  DiagnosticID.cpp
  DiagnosticClient.cpp
  DiagnosticEmitter.cpp
  TextDiagnosticBuffering.cpp
  TextDiagnosticPrinter.cpp
  DiagnosticFormatParser.cpp
  DiagnosticEmitter.cpp

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

target_link_libraries(StoneDiag
  PRIVATE
  ${LLVM_ATOMIC_LIB}
)


