#ifndef STONE_COMPILE_COMPILABLEFILE_H
#define STONE_COMPILE_COMPILABLEFILE_H

#include "stone/Basic/File.h"
#include "stone/Basic/List.h"

#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/Path.h"

namespace stone {

class CompilableFile final {

  bool isPrimary;

  file::File *input = nullptr;
  /// The input, if it comes from a buffer rather than a file. This object
  /// does not own the buffer, and the caller is responsible for ensuring
  /// that it outlives any users.
  llvm::MemoryBuffer *buffer = nullptr;

public:
  CompilableFile() = default;

  CompilableFile(file::File *input, bool isPrimary)
      : CompilableFile(input, isPrimary, nullptr) {}

  /// Constructs an input file from the provided data.
  CompilableFile(file::File *input, bool isPrimary, llvm::MemoryBuffer *buffer)
      : input(input), isPrimary(isPrimary), buffer(buffer) {}

public:
  file::File &GetFile() {
    assert(input && "No File");
    return *input;
  }

  bool IsPrimary() { return isPrimary; }
  /// Retrieves the backing buffer for this input file, if any.
  llvm::MemoryBuffer *GetBuffer() { return buffer; }
};

} // namespace stone
#endif