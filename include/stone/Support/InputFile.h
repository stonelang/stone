#ifndef STONE_SUPPORT_INPUTFILE_H
#define STONE_SUPPORT_INPUTFILE_H

#include "stone/Basic/FileType.h"

namespace stone {

/// Return stone-standard file name from a buffer name set by
/// llvm::MemoryBuffer::getFileOrSTDIN, which uses "<stdin>" instead of "-".
llvm::StringRef ConvertBufferNameFromLLVMGetFileOrSTDINToStoneConventions(
    llvm::StringRef fileName);

class InputFile final {
  llvm::StringRef fileName;
  FileType fileType;

public:
  InputFile(llvm::StringRef fileName)
      : fileName(fileName),
        fileType(stone::GetFileTypeByExt(stone::GetFileExt(fileName))) {}

  /// Constructs an input file from the provided data.
  InputFile(llvm::StringRef fileName, FileType fileType)
      : fileName(fileName), fileType(fileType) {}

public:
  /// The returned file name is guaranteed not to be the empty string.
  const llvm::StringRef &GetFileName() const {
    assert(!fileName.empty());
    return fileName;
  }
  FileType GetFileType() const { return fileType; }
  bool IsStoneFileType() const { return GetFileType() == FileType::Stone; }
  bool IsObjectFileType() const { return GetFileType() == FileType::Object; }
  bool IsIRFileType() const { return GetFileType() == FileType::IR; }
};

} // namespace stone
#endif