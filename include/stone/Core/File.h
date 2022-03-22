#ifndef STONE_CORE_FILE_H
#define STONE_CORE_FILE_H

#include <stdio.h>

#include <functional>

#include "stone/Core/LLVM.h"
#include "llvm/ADT/DenseMapInfo.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/raw_ostream.h"

namespace stone {
namespace file {

enum Type : uint8_t {
#define FILE_TYPE(NAME, TYPE, TEMP_SUFFIX, FLAGS) TYPE,
#include "stone/Core/File.def"
#undef FILE_TYPE
  INVALID
};
/// Return the name of the type for \p Id.
llvm::StringRef GetTypeName(file::Type ty);

/// Return the suffix to use when creating a temp file of this type,
/// or null if unspecified.
llvm::StringRef GetTypeTempSuffix(file::Type ty);

/// Lookup the type to use for the file extension \p Ext.
/// If the extension is empty or is otherwise not recognized, return
/// the invalid type \c TY_INVALID.
file::Type GetTypeByExt(llvm::StringRef Ext);

/// Lookup the type to use for the name \p Name.
file::Type GetTypeByName(llvm::StringRef Name);

/// Returns true if the type represents textual data.
bool IsTextual(file::Type FT);

/// Returns true if the type is produced in the sc after the LLVM
/// passes.
///
/// For those types the sc produces multiple output files in multi-
/// threaded compilation.
bool IsAfterLLVM(file::Type ty);

/// Returns true if the type is a file that contributes to the stone module
/// being compiled.
///
/// These need to be passed to the stone Compile
bool IsPartOfCompilation(stone::file::Type ty);

bool CanCompile(stone::file::Type ty);

/// Returns true if the type is a file that is linkable
///
/// These need to be passed to the stone Compile
bool CanLink(stone::file::Type ty);

bool Exists(llvm::StringRef name);

bool CanExec(std::string path);
llvm::StringRef GetExt(llvm::StringRef name);
llvm::StringRef GetPath(llvm::StringRef path);
llvm::StringRef GetStem(llvm::StringRef path);

template <typename Fn> void forAllTypes(const Fn &fn);

// llvm::StringRef FindProgramByName(llvm::StringRef programName);
// llvm::sys::findProgramByName

class File final {
  file::Type ty;
  unsigned fileID;
  llvm::StringRef name;

public:
  File() = delete;
  File(llvm::StringRef name, file::Type ty, unsigned fileID = 0)
      : name(name), ty(ty), fileID(fileID) {}

public:
  llvm::StringRef GetName() { return name; }
  const llvm::StringRef GetName() const { return name; }
  file::Type GetType() const { return ty; }
  bool OnSystem() { return file::Exists(GetName()); }
  unsigned GetFileID() { return fileID; }
};

/// Type used for a list of input arguments.
using Files = llvm::SmallVector<file::File, 16>;

std::unique_ptr<llvm::raw_pwrite_stream>
CreateOutputFile(llvm::StringRef outFile, std::error_code &error, bool isBinary,
                 bool removeFileOnSignal, llvm::StringRef inFile,
                 llvm::StringRef extension, bool useTemporary,
                 bool createMissingDirectories, std::string *resultPathName,
                 std::string *tempPathName);

} // namespace file

} // namespace stone

namespace llvm {
template <> struct DenseMapInfo<stone::file::Type> {
  using FT = stone::file::Type;
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
      std::is_constructible<std::function<void(stone::file::Type)>, Fn>::value,
      "must have the signature 'void(file::Type)'");
  for (uint8_t i = 0; i < static_cast<uint8_t>(stone::file::Type::INVALID); ++i)
    fn(static_cast<stone::file::Type>(i));
}
#endif
