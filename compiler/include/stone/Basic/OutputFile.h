#ifndef STONE_BASIC_OUTPUTFILE_H
#define STONE_BASIC_OUTPUTFILE_H

#include "stone/Basic/FileQualType.h"
#include "llvm/Support/FileSystem.h"

namespace stone {

struct OutputFile final {
  llvm::StringRef outputFilename;
  std::optional<llvm::sys::fs::TempFile> tempFileName;

public:
  OutputFile(llvm::StringRef outputFilename,
             std::optional<llvm::sys::fs::TempFile> tempFileName)
      : outputFilename(outputFilename), tempFileName(std::move(tempFileName)) {}
};

} // namespace stone
#endif
