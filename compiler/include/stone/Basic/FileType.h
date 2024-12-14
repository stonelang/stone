#ifndef STONE_BASIC_FILE_TYPE_H
#define STONE_BASIC_FILE_TYPE_H

#include "stone/Basic/LLVM.h"

#include "llvm/ADT/DenseMapInfo.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/raw_ostream.h"

#include <functional>
#include <stdio.h>

namespace stone {
namespace filetype {
enum FileType : unsigned {
  None = 0,
#define FILE_TYPE(TYPE, NAME, EXT) TYPE,
#include "stone/Basic/FileType.def"
#undef FILE_TYPE
  INVALID
};
} // namespace filetype
using FileType = filetype::FileType;

/// Return true if this is correct FileType for \p fileType
bool IsAnyFileType(FileType fileType);

/// Return the name of the type for \p Id.
llvm::StringRef GetFileTypeName(FileType fileType);

/// Return the suffix to use when creating a temp file of this type,
/// or null if unspecified.
llvm::StringRef GetFileTypeExt(FileType fileType);

/// Lookup the type to use for the file extension \p Ext.
/// If the extension is empty or is otherwise not recognized, return
/// the invalid type \c TY_INVALID.
FileType GetFileTypeByExt(llvm::StringRef Ext);

/// Lookup the type to use for the name \p Name.
FileType GetFileTypeByName(llvm::StringRef Name);

FileType GetFileTypeByPath(const llvm::StringRef path);

/// Returns true if the type represents textual data.
bool IsTextual(FileType fileType);

/// Returns true if this file type is a valid input file
bool IsInputFileType(FileType fileType);

/// Returns true if this file type is outputable
bool IsOutputFileType(FileType fileType);

/// Returns true if this file type is None
bool IsNoneFileType(FileType fileType);

/// Returns true if this file type is Stone
bool IsStoneFileType(FileType fileType);

/// Returns true if this file type is Object
bool IsObjectFileType(FileType fileType);

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
bool IsPartOfStoneCompilation(FileType fileType);

/// Returns true if the type is a file that contributes being compile by llvm --
/// IR for example.
///
/// These need to be passed to the stone Compile
bool IsPartOfLLVMCompilation(FileType fileType);

bool ShouldCompileFileType(FileType fileType);

/// Returns true if the type is a file that is linkable
///
/// These need to be passed to the stone Compile
bool CanLinkFileType(FileType fileType);

bool FileExists(llvm::StringRef name);

bool CanExecuteFile(std::string path);

/// Get the file extension
llvm::StringRef GetFileExt(llvm::StringRef name);

/// Get the file path
llvm::StringRef GetFilePath(llvm::StringRef path);

/// Get the file stem
llvm::StringRef GetFileStem(llvm::StringRef path);

/// Get the file base
llvm::StringRef GetFileBase(llvm::StringRef input);

} // namespace stone

namespace llvm {
template <> struct DenseMapInfo<stone::FileType> {
  static inline stone::FileType getEmptyKey() {
    return stone::FileType::INVALID;
  }
  static inline stone::FileType getTombstoneKey() {
    return static_cast<stone::FileType>(stone::FileType::INVALID + 1);
  }
  static unsigned getHashValue(stone::FileType Val) {
    return (unsigned)Val * 37U;
  }
  static bool isEqual(stone::FileType LHS, stone::FileType RHS) {
    return LHS == RHS;
  }
};
} // namespace llvm

#endif
