#ifndef STONE_BASIC_VERSION_H
#define STONE_BASIC_VERSION_H

#include "llvm/ADT/StringRef.h"

namespace stone {
class Version final {
public:
  Version() = default;
  /// Retrieves the repository path (e.g., Subversion path) that
  /// identifies the particular stone branch, tag, or trunk from which this
  /// stone was built.
  llvm::StringRef GetRepoPath();

  /// Retrieves the repository revision number (or identifier) from which
  /// this stone was built.
  llvm::StringRef GetRevision();

  /// Retrieves the full repository version that is an amalgamation of
  /// the information in GetRepPath() and GetRevision().
  llvm::StringRef GetFullRepoVersion();

  /// Retrieves a string representing the complete stone version,
  /// which includes the stone version number, the repository version,
  /// and the vendor tag.
  llvm::StringRef GetFullVersion();
};

} // namespace stone

#endif
