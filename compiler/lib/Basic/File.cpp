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
  assert(ty >= 0 && ty < file::INVALID && "Invalid Type ID.");
  return LocalTypes[ty];
}

llvm::StringRef file::GetTypeName(file::Type ty) {
  return GetLocalType(ty).Name;
}

llvm::StringRef file::GetTypeExt(file::Type ty) { return GetLocalType(ty).Ext; }

file::Type file::GetTypeByExt(llvm::StringRef Ext) {
  if (Ext.empty()) {
    return file::INVALID;
  }
  assert(Ext.front() == '.' && "not a file extension");
  return llvm::StringSwitch<file::Type>(Ext.drop_front())
#define FILE_TYPE(NAME, TYPE, EXT, FLAGS) .Case(EXT, TYPE)
#include "stone/Basic/File.def"
      .Default(file::INVALID);
}

file::Type file::GetTypeByName(llvm::StringRef Name) {
  return llvm::StringSwitch<file::Type>(Name)
#define FILE_TYPE(NAME, TYPE, EXT, FLAGS) .Case(NAME, TYPE)
#include "stone/Basic/File.def"
      .Default(file::Type::INVALID);
}

bool file::IsTextual(file::Type ty) {
  switch (ty) {
  case file::Type::Stone:
  case file::Type::Assembly:
  case file::Type::IR:
    return true;
  case file::Type::Image:
  case file::Type::Object:
  case file::Type::BC:
  case file::Type::None:
    return false;
  case file::Type::INVALID:
    llvm_unreachable("Invalid type ID.");
  }

  // Work around MSVC warning: not all control paths return a value
  llvm_unreachable("All switch cases are covered");
}

bool file::IsAfterLLVM(file::Type ty) {
  switch (ty) {
  case file::Type::Assembly:
  case file::Type::IR:
  case file::Type::BC:
  case file::Type::Object:
    return true;
  case file::Type::Stone:
  case file::Type::Image:
  case file::Type::None:
    return false;
  case file::Type::INVALID:
    llvm_unreachable("Invalid type ID.");
  }

  // Work around MSVC warning: not all control paths return a value
  llvm_unreachable("All switch cases are covered");
}

bool file::IsPartOfCompilation(file::Type ty) {
  switch (ty) {
  case file::Type::Stone:
    return true;
  case file::Type::Assembly:
  case file::Type::IR:
  case file::Type::BC:
  case file::Type::Object:
  case file::Type::Image:
  case file::Type::None:
    return false;
  case file::Type::INVALID:
    llvm_unreachable("Unknown type.");
  }
  // Work around MSVC warning: not all control paths return a value
  llvm_unreachable("All switch cases are covered");
}

/// Returns true if the type is a file that is linkable
///
/// These need to be passed to the stone Compile
bool file::CanCompile(stone::file::Type ty) {
  switch (ty) {
  case file::Type::Stone:
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
bool file::CanLink(stone::file::Type ty) {
  switch (ty) {
  case file::Type::Object:
  case file::Type::Image:
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
