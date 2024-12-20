#include "stone/Basic/Version.h"

using stone::Version;

/// Retrieves the repository path (e.g., Subversion path) that
/// identifies the particular stone branch, tag, or trunk from which this
/// stone was built.
llvm::StringRef Version::GetRepoPath() { return llvm::StringRef(); }

/// Retrieves the repository revision number (or identifier) from which
/// this stone was built.
llvm::StringRef Version::GetRevision() { return llvm::StringRef(); }

/// Retrieves the full repository version that is an amalgamation of
/// the information in GetRepPath() and GetRevision().
llvm::StringRef Version::GetFullRepoVersion() { return llvm::StringRef(); }

/// Retrieves a string representing the complete stone version,
/// which includes the stone version number, the repository version,
/// and the vendor tag.
llvm::StringRef Version::GetFullVersion() { return llvm::StringRef(); }