#ifndef STONE_CORE_SRCFILE_H
#define STONE_CORE_SRCFILE_H

#include "llvm/ADT/StringRef.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/VirtualFileSystem.h"

#include <ctime>
#include <map>
#include <string>

namespace stone {

/// Cached information about one directory (either on disk or in
/// the virtual file system).
class SrcDir {
  friend class FileMgr;
  llvm::StringRef Name; // Name of the directory.

public:
  llvm::StringRef getName() const { return Name; }
};

/// Cached information about one file (either on disk
/// or in the virtual file system).
///
/// If the 'File' member is valid, then this SrcFile has an open file
/// descriptor for the file.
class SrcFile {
  friend class FileMgr;

  StringRef Name;           // Name of the file.
  std::string RealPathName; // Real path to the file; could be empty.
  off_t Size;               // File size in bytes.
  time_t ModTime;           // Modification time of file.
  const SrcDir *Dir;        // Directory file lives in.
  unsigned UID;             // A unique (small) ID for the file.
  llvm::sys::fs::UniqueID UniqueID;
  bool IsNamedPipe;
  bool IsValid; // Is this \c SrcFile initialized and valid?

  /// The open file, if it is owned by the \p SrcFile.
  mutable std::unique_ptr<llvm::vfs::File> File;

public:
  SrcFile() : UniqueID(0, 0), IsNamedPipe(false), IsValid(false) {}

  SrcFile(const SrcFile &) = delete;
  SrcFile &operator=(const SrcFile &) = delete;

  StringRef getName() const { return Name; }
  StringRef tryGetRealPathName() const { return RealPathName; }
  bool isValid() const { return IsValid; }
  off_t getSize() const { return Size; }
  unsigned getUID() const { return UID; }
  const llvm::sys::fs::UniqueID &getUniqueID() const { return UniqueID; }
  time_t getModificationTime() const { return ModTime; }

  /// Return the directory the file lives in.
  const SrcDir *getDir() const { return Dir; }

  bool operator<(const SrcFile &RHS) const { return UniqueID < RHS.UniqueID; }

  /// Check whether the file is a named pipe (and thus can't be opened by
  /// the native FileMgr methods).
  bool isNamedPipe() const { return IsNamedPipe; }

  void closeFile() const {
    File.reset(); // rely on destructor to close File
  }

  // Only for use in tests to see if deferred opens are happening, rather than
  // relying on RealPathName being empty.
  bool isOpenForTests() const { return File != nullptr; }
};

} // namespace stone

#endif