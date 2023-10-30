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
	EditorPlaceholder.cpp
	File.cpp
	FileEntry.cpp
	FileMgr.cpp
	FileSystem.cpp
	FileSystemStatCache.cpp
	JSONSerialization.cpp
	OutputFileMap.cpp
	PlatformKind.cpp
	SrcMgr.cpp
	LangOptions.cpp
	OperatorKind.cpp
	PlatformKind.cpp
	StatisticEngine.cpp
	StableHasher.cpp
	Timer.cpp
	TokenKind.cpp
	Version.cpp

)
add_stone_library(StoneBasic
	${stone_basic_sources}
)



