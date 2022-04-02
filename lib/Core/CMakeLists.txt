set( LLVM_LINK_COMPONENTS
  ${LLVM_TARGETS_TO_BUILD}
	BitReader
	BinaryFormat
  BitstreamReader
  Support
	Core
  )
set(stone_ctx_sources
	Context.cpp
	BumpTable.cpp
	Char.cpp
	DarwinDirWatcher.cpp
	DiagIDList.cpp
	Diagnostic.cpp
	DiagnosticEmitter.cpp
	DiagnosticFormatter.cpp
	DiagnosticEngine.cpp
	DiagnosticListener.cpp
	DiagnosticTransaction.cpp
	DirScanner.cpp
	DirWatcher.cpp
	EditorPlaceholder.cpp
	File.cpp
	FileEntry.cpp
	FileMgr.cpp
	FileSystem.cpp
	FileSystemStatCache.cpp
	LLVMContext.cpp
	OutputFileMap.cpp
	SrcMgr.cpp
	SystemOptions.cpp
	StatisticEngine.cpp
	TextDiagnosticEmitter.cpp
	TextDiagnosticFormatter.cpp
	TextDiagnosticListener.cpp
	Timer.cpp
	TokenKind.cpp
	UnixDirWatcher.cpp
	Version.cpp
	WinDirWatcher.cpp
)
add_stone_library(StoneCore
	${stone_ctx_sources}
)



