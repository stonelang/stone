#include "stone/Basic/LangVersion.h"

using stone::LangVersion;

/// Retrieves the repository path (e.g., Subversion path) that
/// identifies the particular stone branch, tag, or trunk from which this
/// stone was built.
llvm::StringRef LangVersion::GetRepoPath() { return llvm::StringRef(); }

/// Retrieves the repository revision number (or identifier) from which
/// this stone was built.
llvm::StringRef LangVersion::GetRevision() { return llvm::StringRef(); }

/// Retrieves the full repository version that is an amalgamation of
/// the information in GetRepPath() and GetRevision().
llvm::StringRef LangVersion::GetFullRepoVersion() { return llvm::StringRef(); }

/// Retrieves a string representing the complete stone version,
/// which includes the stone version number, the repository version,
/// and the vendor tag.
llvm::StringRef LangVersion::GetFullVersion() { return llvm::StringRef(); }