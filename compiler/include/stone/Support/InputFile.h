#ifndef STONE_OPTION_INPUTFILE_H
#define STONE_OPTION_INPUTFILE_H

#include "stone/Basic/FileQualType.h"

namespace stone {

class InputFile final {
  llvm::StringRef fileName;
  stone::FileType fileType;

public:
  InputFile(llvm::StringRef fileName)
      : InputFile(fileName, file::GetTypeByExt(file::GetExt(fileName))) {}

  /// Constructs an input file from the provided data.
  InputFile(llvm::StringRef fileName, stone::FileType fileType)
      : fileName(ConvertBufferNameFromLLVMGetFileOrSTDINToStoneConventions(
            fileName)),
        fileType(fileType) {

    assert(!fileName.empty() && fileName.size() > 0);
  }

public:
  /// Retrieves the type of this input file.
  // stone::FileType GetFileType() const { return fileType; };
  /// The name of this \c CompilerInputFile, or `-` if this input corresponds to
  /// the standard input stream.
  ///
  /// The returned file name is guaranteed not to be the empty string.
  const llvm::StringRef &GetFileName() const {
    assert(!fileName.empty());
    return fileName;
  }
  const stone::FileType GetFileType() const { return fileType; }

  bool IsPartOfStoneCompilation() const {
    return file::IsPartOfStoneCompilation(GetFileType());
  }
  bool IsStoneFileType() const {
    return GetFileType() == stone::FileType::Stone;
  }
  bool IsObjectFileType() const {
    return GetFileType() == stone::FileType::Object;
  }
  bool IsAutolinkFileType() const {
    return GetFileType() == stone::FileType::Autolink;
  }
  bool IsStoneModuleFileType() const {
    return GetFileType() == stone::FileType::StoneModule;
  }
  bool HasValidInputFileType() const {
    return (IsStoneFileType() || IsObjectFileType() ||
            IsStoneModuleFileType() || IsAutolinkFileType());
  }

public:
  /// Return stone-standard file name from a buffer name set by
  /// llvm::MemoryBuffer::getFileOrSTDIN, which uses "<stdin>" instead of "-".
  static llvm::StringRef
  ConvertBufferNameFromLLVMGetFileOrSTDINToStoneConventions(
      llvm::StringRef fileName) {
    if (fileName.equals("<stdin>")) {
      return "-";
    }
    return fileName;
  }
};

} // namespace stone
#endif
