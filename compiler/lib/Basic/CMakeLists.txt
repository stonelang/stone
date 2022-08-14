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
set(stone_basic_sources
	Cache.cpp
	BumpTable.cpp
	Char.cpp
	DarwinDirWatcher.cpp
	DirScanner.cpp
	DirWatcher.cpp
	EditorPlaceholder.cpp
	File.cpp
	FileEntry.cpp
	FileMgr.cpp
	FileSystem.cpp
	FileSystemStatCache.cpp
	JSONSerialization.cpp
	LLVMContext.cpp
	OutputFileMap.cpp
	SrcMgr.cpp
	LangOptions.cpp
	StatisticEngine.cpp
	Timer.cpp
	TokenKind.cpp
	UnixDirWatcher.cpp
	Version.cpp
	WinDirWatcher.cpp
)
add_stone_library(StoneBasic
	${stone_basic_sources}
)



