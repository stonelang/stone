#ifndef STONE_GEN_CODEGENOPTIONS_H
#define STONE_GEN_CODEGENOPTIONS_H

#include "llvm/ADT/StringRef.h"

namespace stone {

enum class LibraryKind { Library = 0, Framework };

class LinkLibrary final {
private:
  std::string name;
  unsigned kind : 1;
  unsigned forceLoad : 1;

public:
  LinkLibrary(llvm::StringRef name, LibraryKind kind, bool forceLoad = false)
      : name(name), kind(static_cast<unsigned>(kind)), forceLoad(forceLoad) {
    assert(GetKind() == kind && "not enough bits for the kind");
  }

  LibraryKind GetKind() const { return static_cast<LibraryKind>(kind); }
  llvm::StringRef GetName() const { return name; }
  bool ShouldForceLoad() const { return forceLoad; }
};

class CodeGenOptions final {
public:
  bool skipOptimization = false;

  /// The libraries and frameworks specified on the command line.
  llvm::SmallVector<LinkLibrary, 4> linkLibraries;

  /// The public dependent libraries specified on the command line.
  std::vector<std::string> publicLinkLibraries;
};

} // namespace stone

#endif
