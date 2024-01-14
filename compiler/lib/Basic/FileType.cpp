#include "stone/Basic/FileType.h"

#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Path.h"

using namespace stone;
using namespace stone::file;

struct FileTypeInfo {
  const char *Name;
  const char *Flags;
  const char *Ext;
};

static const FileTypeInfo FileTypeInfos[] = {
#define FILE_TYPE(NAME, TYPE, EXT, FLAGS) {NAME, FLAGS, EXT},
#include "stone/Basic/FileType.def"
};

static const FileTypeInfo &GetFileTypeInfo(unsigned ty) {
  assert(ty >= 0 && ty < FileType::INVALID && "Invalid Type ID.");
  return FileTypeInfos[ty];
}

llvm::StringRef file::GetTypeName(FileType ty) {
  return GetFileTypeInfo(ty).Name;
}

llvm::StringRef file::GetTypeExt(FileType ty) {
  return GetFileTypeInfo(ty).Ext;
}

FileType file::GetTypeByExt(llvm::StringRef Ext) {
  if (Ext.empty()) {
    return file::INVALID;
  }
  assert(Ext.front() == '.' && "not a file extension");
  return llvm::StringSwitch<FileType>(Ext.drop_front())
#define FILE_TYPE(NAME, TYPE, EXT, FLAGS) .Case(EXT, TYPE)
#include "stone/Basic/FileType.def"
      .Default(file::INVALID);
}

FileType file::GetTypeByName(llvm::StringRef Name) {
  return llvm::StringSwitch<FileType>(Name)
#define FILE_TYPE(NAME, TYPE, EXT, FLAGS) .Case(NAME, TYPE)
#include "stone/Basic/FileType.def"
      .Default(FileType::INVALID);
}

file::FileType file::GetTypeByPath(const llvm::StringRef path) {
  if (!llvm::sys::path::has_extension(path)) {
    return stone::file::FileType::INVALID;
  }
  auto ext = llvm::sys::path::extension(path).str();
  auto fileType = stone::file::GetTypeByExt(ext);
  if (fileType == stone::file::FileType::INVALID) {
    auto pathStem = llvm::sys::path::stem(path);
    // If this path has a multiple '.' extension (e.g. .abi.json),
    // then iterate over all preceeding possible extension variants.
    while (llvm::sys::path::has_extension(pathStem)) {
      auto nextExtension = llvm::sys::path::extension(pathStem);
      pathStem = llvm::sys::path::stem(pathStem);
      ext = nextExtension.str() + ext;
      fileType = stone::file::GetTypeByExt(ext);
      if (fileType != stone::file::FileType::INVALID) {
        break;
      }
    }
  }
  return fileType;
}

bool file::IsTextual(FileType ty) {
  switch (ty) {
  case FileType::Stone:
  case FileType::Assembly:
  case FileType::IR:
    return true;
  case FileType::Image:
  case FileType::Object:
  case FileType::BC:
  case FileType::None:
    return false;
  case FileType::INVALID:
    llvm_unreachable("Invalid type ID.");
  }

  // Work around MSVC warning: not all control paths return a value
  llvm_unreachable("All switch cases are covered");
}

/// Returns true if this file type is None
bool file::IsNoneFileType(FileType fileType) {
  return fileType = FileType::None;
}

/// Returns true if this file type is Stone
bool file::IsStoneFileType(FileType fileType) {
  return fileType = FileType::Stone;
}

/// Returns true if this file type is Object
bool file::IsObjectFileType(FileType fileType) {
  return fileType = FileType::Object;
}

/// Returns true if this file type is outputable
bool file::IsOutputFileType(FileType fileType) {

  switch (fileType) {
  case FileType::Assembly:
  case FileType::IR:
  case FileType::BC:
  case FileType::Object:
  case FileType::Image:
    return true;
  case FileType::Stone:
    return false;
  case FileType::INVALID:
    llvm_unreachable("Invalid type ID.");
  }
}

/// Returns true if this file type is outputable
bool file::IsInputFileType(FileType fileType) {
  switch (fileType) {
  case FileType::Stone:
  case FileType::IR:
  case FileType::Object:
  case FileType::StoneModule:
  case FileType::Autolink:
    return true;
  case FileType::Assembly:
  case FileType::Image:
  case FileType::BC:
    return false;
  }
  llvm_unreachable("Invalid type ID.");
}

bool file::IsAfterLLVM(FileType ty) {
  switch (ty) {
  case FileType::Assembly:
  case FileType::IR:
  case FileType::BC:
  case FileType::Object:
    return true;
  case FileType::Stone:
  case FileType::Image:
  case FileType::None:
    return false;
  case FileType::INVALID:
    llvm_unreachable("Invalid type ID.");
  }

  // Work around MSVC warning: not all control paths return a value
  llvm_unreachable("All switch cases are covered");
}

bool file::IsPartOfStoneCompilation(FileType ty) {
  switch (ty) {
  case FileType::Stone:
    return true;
  case FileType::Assembly:
  case FileType::IR:
  case FileType::BC:
  case FileType::Object:
  case FileType::Image:
  case FileType::None:
    return false;
  case FileType::INVALID:
    llvm_unreachable("Unknown type.");
  }
  // Work around MSVC warning: not all control paths return a value
  llvm_unreachable("All switch cases are covered");
}

bool file::IsPartOfLLVMCompilation(FileType ty) {
  switch (ty) {
  case FileType::IR:
  case FileType::BC:
    return true;
  case FileType::Stone:
  case FileType::Assembly:
  case FileType::Object:
  case FileType::Image:
  case FileType::None:
    return false;
  case FileType::INVALID:
    llvm_unreachable("Unknown type.");
  }
  // Work around MSVC warning: not all control paths return a value
  llvm_unreachable("All switch cases are covered");
}

/// Returns true if the type is a file that is linkable
///
/// These need to be passed to the stone Compile
bool file::ShouldCompile(file::FileType ty) {
  switch (ty) {
  case FileType::Stone:
    return true;
  default:
    return false;
  }
  // Work around MSVC warning: not all control paths return a value
  llvm_unreachable("All switch cases are covered");
}

/// Returns true if the type is a file that is linkable
///
/// These need to be passed to the stone Compile
bool file::CanLink(file::FileType ty) {
  switch (ty) {
  case FileType::Object:
  case FileType::Image:
    return true;
  default:
    return false;
  }
  // Work around MSVC warning: not all control paths return a value
  llvm_unreachable("All switch cases are covered");
}

bool file::Exists(llvm::StringRef name) {
  if (llvm::sys::fs::exists(name)) {
    return true;
  }
}

llvm::StringRef file::GetBase(llvm::StringRef name) {
  llvm::sys::path::filename(name);
}

llvm::StringRef file::GetExt(llvm::StringRef name) {
  return llvm::sys::path::extension(name);
}
llvm::StringRef file::GetPath(llvm::StringRef name) {
  return llvm::sys::path::root_name(name);
}

llvm::StringRef file::GetStem(llvm::StringRef path) {
  return llvm::sys::path::stem(path);
}
bool file::CanExec(std::string path) {
  return llvm::sys::fs::can_execute(llvm::Twine(path));
}
