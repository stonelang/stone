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
	About.cpp
	Cache.cpp
	BumpTable.cpp
	EditorPlaceholder.cpp
	FileType.cpp
	FileSystem.cpp
	FileSystemStatCache.cpp
	JSONSerialization.cpp
	Memory.cpp
	OutputFileMap.cpp
	PlatformKind.cpp
	SrcMgr.cpp
	LangOptions.cpp
	OperatorKind.cpp
	PlatformKind.cpp
	StableHasher.cpp
	Timer.cpp
	TokenKind.cpp
	Version.cpp

)
add_stone_library(StoneBasic
	${stone_basic_sources}
)



