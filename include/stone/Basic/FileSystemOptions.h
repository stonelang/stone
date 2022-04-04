#ifndef STONE_BASIC_FILESYSTEMOPTIONS_H
#define STONE_BASIC_FILESYSTEMOPTIONS_H

#include <string>

#include "llvm/ADT/StringRef.h"

namespace stone {

class FileSystemOptions final {
public:
  // Sometimes empty() on std::string returns characters when we are
  // passing things around by ref.
  bool UseWorkingDir = false;

  /// The working directory to use
  std::string WorkingDir;

public:
  FileSystemOptions() {}
};

} // namespace stone

#endif
