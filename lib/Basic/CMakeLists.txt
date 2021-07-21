set( LLVM_LINK_COMPONENTS
  ${LLVM_TARGETS_TO_BUILD}
	BitReader
	BinaryFormat
  BitstreamReader
  Support
	Core
  )
set(stone_utils_sources
	Basic.cpp
	BumpTable.cpp
	Char.cpp
	Clock.cpp
	DarwinDirWatcher.cpp
	DiagIDList.cpp
	Diagnostic.cpp
	DiagnosticEmitter.cpp
	DiagnosticFormatter.cpp
	DiagnosticEngine.cpp
	DiagnosticListener.cpp
	DirScanner.cpp
	DirWatcher.cpp
	File.cpp
	FileMgr.cpp
	FileSystemStatCache.cpp
	Fmt.cpp
	LangVersion.cpp
	LLVMContext.cpp
	PipelineEngine.cpp
	SrcLoc.cpp
	SrcMgr.cpp
	Stats.cpp
	TextDiagnosticEmitter.cpp
	TextDiagnosticFormatter.cpp
	TextDiagnosticListener.cpp
	TokenType.cpp
	UnixDirWatcher.cpp
	WinDirWatcher.cpp
)
add_stone_library(StoneBasic
	${stone_utils_sources}
)



