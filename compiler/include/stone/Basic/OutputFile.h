#ifndef STONE_BASIC_OUTPUTFILE_H
#define STONE_BASIC_OUTPUTFILE_H

#include "stone/Basic/File.h"
#include "llvm/Support/FileSystem.h"

namespace stone {

class OutputFile final {
  file::File &output;
  llvm::Optional<llvm::sys::fs::TempFile> tempFile;

  OutputFile() = delete;
  OutputFile(const OutputFile &) = delete;
  OutputFile &operator=(const OutputFile &) = delete;

public:
  OutputFile(file::File &output,
             llvm::Optional<llvm::sys::fs::TempFile> tempFile)
      : output(output), tempFile(std::move(tempFile)) {}

public:
  file::File &GetFile() const { return output; }
  llvm::Optional<llvm::sys::fs::TempFile> &GetTempFile() { return tempFile; }

  // std::unique_ptr<llvm::raw_pwrite_stream> ToOutputStream();
};

} // namespace stone
#endif
