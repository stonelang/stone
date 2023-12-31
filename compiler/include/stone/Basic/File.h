#ifndef STONE_BASIC_FILE_H
#define STONE_BASIC_FILE_H
#include "stone/Basic/LLVM.h"

#include "llvm/ADT/DenseMapInfo.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/raw_ostream.h"

#include <functional>
#include <stdio.h>

namespace stone {
namespace file {

enum FileType : uint8_t {
#define FILE_TYPE(NAME, TYPE, EXT, FLAGS) TYPE,
#include "stone/Basic/File.def"
#undef FILE_TYPE
  INVALID
};
/// Return the name of the type for \p Id.
llvm::StringRef GetTypeName(FileType ty);

/// Return the suffix to use when creating a temp file of this type,
/// or null if unspecified.
llvm::StringRef GetTypeExt(FileType ty);

/// Lookup the type to use for the file extension \p Ext.
/// If the extension is empty or is otherwise not recognized, return
/// the invalid type \c TY_INVALID.
FileType GetTypeByExt(llvm::StringRef Ext);

/// Lookup the type to use for the name \p Name.
FileType GetTypeByName(llvm::StringRef Name);

/// Returns true if the type represents textual data.
bool IsTextual(FileType fileType);

/// Returns true if this file type is a valid input file
bool IsInputableFileType(FileType fileType);

/// Returns true if this file type is outputable
bool IsOutputableFileType(FileType fileType);

/// Returns true if the type is produced in the sc after the LLVM
/// passes.
///
/// For those types the sc produces multiple output files in multi-
/// threaded compilation.
bool IsAfterLLVM(FileType fileType);

/// Returns true if the type is a file that contributes to the stone module
/// being compiled.
///
/// These need to be passed to the stone Compile
bool IsPartOfCompilation(FileType fileType);

bool CanCompile(FileType fileType);

/// Returns true if the type is a file that is linkable
///
/// These need to be passed to the stone Compile
bool CanLink(FileType fileType);

bool Exists(llvm::StringRef name);

bool CanExec(std::string path);

llvm::StringRef GetExt(llvm::StringRef name);
llvm::StringRef GetPath(llvm::StringRef path);
llvm::StringRef GetStem(llvm::StringRef path);
llvm::StringRef GetBase(llvm::StringRef input);

template <typename Fn> void forAllTypes(const Fn &fn);

// llvm::StringRef FindProgramByName(llvm::StringRef programName);
// llvm::sys::findProgramByName

class File final {
  FileType ty;
  unsigned fileID;
  llvm::StringRef name;

public:
  File() = delete;
  File(llvm::StringRef name, FileType ty, unsigned fileID = 0)
      : name(name), ty(ty), fileID(fileID) {}

public:
  llvm::StringRef GetName() { return name; }
  const llvm::StringRef GetName() const { return name; }
  FileType GetType() const { return ty; }
  bool OnSystem() { return file::Exists(GetName()); }
  unsigned GetFileID() { return fileID; }
};

/// Type used for a list of input arguments.
using Files = llvm::SmallVector<file::File, 16>;

std::unique_ptr<llvm::raw_pwrite_stream>
CreateOutputStream(llvm::StringRef outFile, std::error_code &error,
                   bool isBinary, bool removeFileOnSignal,
                   llvm::StringRef inFile, llvm::StringRef extension,
                   bool useTemporary, bool createMissingDirectories,
                   std::string *resultPathName, std::string *tempPathName);

} // namespace file

} // namespace stone

namespace llvm {
template <> struct DenseMapInfo<stone::file::FileType> {
  using FT = stone::file::FileType;
  static inline FT getEmptyKey() { return FT::INVALID; }
  static inline FT getTombstoneKey() {
    return static_cast<FT>(FT::INVALID + 1);
  }
  static unsigned getHashValue(FT Val) { return (unsigned)Val * 37U; }
  static bool isEqual(FT LHS, FT RHS) { return LHS == RHS; }
};
} // namespace llvm

template <typename Fn> void stone::file::forAllTypes(const Fn &fn) {
  static_assert(
      std::is_constructible<std::function<void(stone::file::FileType)>,
                            Fn>::value,
      "must have the signature 'void(FileType)'");
  for (uint8_t i = 0; i < static_cast<uint8_t>(stone::file::FileType::INVALID);
       ++i)
    fn(static_cast<stone::file::FileType>(i));
}
#endif
