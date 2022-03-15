#include "stone/Core/FileEntry.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/VirtualFileSystem.h"

using namespace stone;

FileEntry::FileEntry() : UniqueID(0, 0) {}

FileEntry::~FileEntry() = default;

void FileEntry::closeFile() const { File.reset(); }
