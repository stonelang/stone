#ifndef STONE_UTILS_OUTPUTFILE_H
#define STONE_UTILS_OUTPUTFILE_H

#include "stone/Utils/File.h"
#include "llvm/Support/FileSystem.h"

namespace stone {

class OutputFile final {
  file::File &output;
  llvm::Optional<llvm::sys::fs::TempFile> tempFile;

public:
  OutputFile(file::File &output,
             llvm::Optional<llvm::sys::fs::TempFile> tempFile)
      : output(output), tempFile(std::move(tempFile)) {}

public:
  file::File &GetFile() { return output; }
  llvm::Optional<llvm::sys::fs::TempFile> &GetTemFile() { return tempFile; }
};
} // namespace stone
#endif
