#ifndef STONE_DRIVER_DRIVER_INPUTFILE_H
#define STONE_DRIVER_DRIVER_INPUTFILE_H

#include "stone/Basic/File.h"
#include "stone/Driver/CompilationEntity.h"

namespace stone {

class DriverInputFile final : public CompilationEntity {
  llvm::StringRef fileName;

public:
  DriverInputFile(llvm::StringRef fileName)
      : DriverInputFile(fileName, file::GetTypeByExt(file::GetExt(fileName))) {}

  /// Constructs an input file from the provided data.
  DriverInputFile(llvm::StringRef fileName, file::FileType fileType)
      : CompilationEntity(CompilationEntityKind::Input, fileType),
        fileName(ConvertBufferNameFromLLVMGetFileOrSTDINToStoneConventions(
            fileName)) {

    assert(!fileName.empty() && fileName.size() > 0);
    AddAllowFileType();
  }

public:
  /// Retrieves the type of this input file.
  // file::FileType GetFileType() const { return fileType; };
  /// The name of this \c CompilerInputFile, or `-` if this input corresponds to
  /// the standard input stream.
  ///
  /// The returned file name is guaranteed not to be the empty string.
  const llvm::StringRef &GetFileName() const {
    assert(!fileName.empty());
    return fileName;
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

public:
  static bool classof(const CompilationEntity *entity) {
    return (entity->GetKind() == CompilationEntityKind::Input);
  }

public:
  static DriverInputFile *Create(const Driver &driver,
                                 llvm::StringRef fileName);
  static DriverInputFile *Create(const Driver &driver, llvm::StringRef fileName,
                                 file::FileType fileType);
};

} // namespace stone
#endif