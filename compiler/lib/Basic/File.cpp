#include "stone/Basic/File.h"

#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Path.h"

using namespace stone;
using namespace stone::file;

struct LocalType {
  const char *Name;
  const char *Flags;
  const char *Ext;
};

static const LocalType LocalTypes[] = {
#define FILE_TYPE(NAME, TYPE, EXT, FLAGS) {NAME, FLAGS, EXT},
#include "stone/Basic/File.def"
};

static const LocalType &GetLocalType(unsigned ty) {
  assert(ty >= 0 && ty < FileType::INVALID && "Invalid Type ID.");
  return LocalTypes[ty];
}

llvm::StringRef file::GetTypeName(FileType ty) { return GetLocalType(ty).Name; }

llvm::StringRef file::GetTypeExt(FileType ty) { return GetLocalType(ty).Ext; }

FileType file::GetTypeByExt(llvm::StringRef Ext) {
  if (Ext.empty()) {
    return file::INVALID;
  }
  assert(Ext.front() == '.' && "not a file extension");
  return llvm::StringSwitch<FileType>(Ext.drop_front())
#define FILE_TYPE(NAME, TYPE, EXT, FLAGS) .Case(EXT, TYPE)
#include "stone/Basic/File.def"
      .Default(file::INVALID);
}

FileType file::GetTypeByName(llvm::StringRef Name) {
  return llvm::StringSwitch<FileType>(Name)
#define FILE_TYPE(NAME, TYPE, EXT, FLAGS) .Case(NAME, TYPE)
#include "stone/Basic/File.def"
      .Default(FileType::INVALID);
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

/// Returns true if this file type is outputable
bool file::IsOutputableFileType(FileType fileType) {

  switch (fileType) {
  case FileType::Assembly:
  case FileType::IR:
  case FileType::BC:
  case FileType::Object:
    return true;
  case FileType::Stone:
    return false;
  case FileType::INVALID:
    llvm_unreachable("Invalid type ID.");
  }
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

bool file::IsPartOfCompilation(FileType ty) {
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

/// Returns true if the type is a file that is linkable
///
/// These need to be passed to the stone Compile
bool file::CanCompile(file::FileType ty) {
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
