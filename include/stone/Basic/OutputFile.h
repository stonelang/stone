#ifndef STONE_BASIC_OUTPUTFILE_H
#define STONE_BASIC_OUTPUTFILE_H

#include "stone/Basic/File.h"
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
  llvm::Optional<llvm::sys::fs::TempFile> &GetTempFile() { return tempFile; }

  // llvm::Optional<raw_fd_ostream> GetOutputFileStream() {
  //   if (!output.GetFileName().empty()) {

  //     llvm::Optional<raw_fd_ostream> outputFileStream;
  //     // Try to open the output file.  Clobbering an existing file is fine.
  //     // Open in binary mode if we're doing binary output.
  //     llvm::sys::fs::OpenFlags openFlags = llvm::sys::fs::OF_None;
  //     std::error_code ec;
  //     outputFileStream.emplace(!output.GetFileName(), ec, openFlags);

  // }
  // return llvm::None;
};

} // namespace stone
#endif
