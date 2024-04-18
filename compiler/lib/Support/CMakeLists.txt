set( LLVM_LINK_COMPONENTS
  ${LLVM_TARGETS_TO_BUILD}
	BitReader
	BinaryFormat
  BitstreamReader
  Support
  Core
  IPO
  Option
 )
set(stone_support_sources
  DiagIDList.cpp
  Diagnostic.cpp
  DiagnosticEmitter.cpp
  DiagnosticFormatter.cpp
  DiagnosticEngine.cpp
  DiagnosticConsumer.cpp
  StatisticEngine.cpp
  TextDiagnosticEmitter.cpp
  TextDiagnosticFormatter.cpp
  TextDiagnosticConsumer.cpp

	LINK_LIBS 
	StoneBasic
)
add_stone_library(StoneSupport
	${stone_support_sources}
)
