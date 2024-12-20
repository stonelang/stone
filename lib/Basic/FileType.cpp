#include "stone/Basic/FileType.h"

#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Path.h"

using namespace stone;
using namespace stone::filetype;

struct FileTypeInfo final {
  const FileType Type;
  const char *Name;
  const char *Ext;
};

static const FileTypeInfo FileTypeInfoList[] = {
#define FILE_TYPE(TYPE, NAME, EXT) {FileType::TYPE, NAME, EXT},
#include "stone/Basic/FileType.def"
#undef FILE_TYPE
};

static const FileTypeInfo &GetFileTypeInfo(FileType fileType) {
  assert(stone::IsAnyFileType(fileType) && "Invalid FileType.");
  return FileTypeInfoList[static_cast<unsigned>(fileType)];
}

bool stone::IsAnyFileType(FileType fileType) {
  switch (fileType) {
  case FileType::Stone:
  case FileType::IR:
  case FileType::Object:
  case FileType::Assembly:
  case FileType::Image:
  case FileType::BC:
  case FileType::StoneModuleFile:
    return true;
  case FileType::None:
  case FileType::INVALID:
    return false;
  }
  return false;
}

llvm::StringRef stone::GetFileTypeName(FileType fileType) {
  return GetFileTypeInfo(fileType).Name;
}

llvm::StringRef stone::GetFileTypeExt(FileType fileType) {
  return GetFileTypeInfo(fileType).Ext;
}

FileType stone::GetFileTypeByExt(const llvm::StringRef ext) {
  if (ext.empty()) {
    return FileType::INVALID;
  }
  assert(ext.front() == '.' && "not a file extension");
  return llvm::StringSwitch<FileType>(ext.drop_front())
#define FILE_TYPE(TYPE, NAME, EXT) .Case(EXT, FileType::TYPE)
#include "stone/Basic/FileType.def"
      .Default(FileType::INVALID);
}

FileType stone::GetFileTypeByName(const llvm::StringRef name) {
  if (name.empty()) {
    return FileType::INVALID;
  }
  return llvm::StringSwitch<FileType>(name)
#define FILE_TYPE(TYPE, NAME, EXT) .Case(NAME, FileType::TYPE)
#include "stone/Basic/FileType.def"
      .Default(FileType::INVALID);
}

FileType stone::GetFileTypeByPath(const llvm::StringRef path) {
  if (!llvm::sys::path::has_extension(path)) {
    return FileType::INVALID;
  }
  auto ext = llvm::sys::path::extension(path).str();
  auto fileType = stone::GetFileTypeByExt(ext);
  if (fileType == FileType::INVALID) {
    auto pathStem = llvm::sys::path::stem(path);
    // If this path has a multiple '.' extension (e.g. .abi.json),
    // then iterate over all preceeding possible extension variants.
    while (llvm::sys::path::has_extension(pathStem)) {
      auto nextExtension = llvm::sys::path::extension(pathStem);
      pathStem = llvm::sys::path::stem(pathStem);
      ext = nextExtension.str() + ext;
      fileType = stone::GetFileTypeByExt(ext);
      if (fileType != FileType::INVALID) {
        break;
      }
    }
  }
  return fileType;
}

bool stone::IsTextual(FileType ty) {
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
bool stone::IsNoneFileType(FileType fileType) {
  return fileType == FileType::None;
}

/// Returns true if this file type is Stone
bool stone::IsStoneFileType(FileType fileType) {
  return fileType == FileType::Stone;
}

/// Returns true if this file type is Object
bool stone::IsObjectFileType(FileType fileType) {
  return fileType == FileType::Object;
}

/// Returns true if this file type is outputable
bool stone::IsOutputFileType(FileType fileType) {

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
bool stone::IsInputFileType(FileType fileType) {
  switch (fileType) {
  case FileType::Stone:
  case FileType::IR:
  case FileType::Object:
  case FileType::StoneModuleFile:
  case FileType::Autolink:
    return true;
  case FileType::Assembly:
  case FileType::Image:
  case FileType::BC:
    return false;
  }
  llvm_unreachable("Invalid type ID.");
}

bool stone::IsAfterLLVM(FileType ty) {
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

bool stone::IsPartOfStoneCompilation(FileType ty) {
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

bool stone::IsPartOfLLVMCompilation(FileType ty) {
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
bool stone::ShouldCompileFileType(FileType ty) {
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
bool stone::CanLinkFileType(FileType ty) {
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

bool stone::FileExists(llvm::StringRef name) {
  if (llvm::sys::fs::exists(name)) {
    return true;
  }
  return false;
}

llvm::StringRef stone::GetFileExt(llvm::StringRef name) {
  return llvm::sys::path::extension(name);
}
llvm::StringRef stone::GetFilePath(llvm::StringRef name) {
  return llvm::sys::path::root_name(name);
}

llvm::StringRef stone::GetFileStem(llvm::StringRef path) {
  return llvm::sys::path::stem(path);
}
bool stone::CanExecuteFile(std::string path) {
  return llvm::sys::fs::can_execute(llvm::Twine(path));
}
