#include "stone/Driver/CompilationEntity.h"
#include "stone/Driver/Driver.h"
#include "stone/Driver/DriverPrettyStackTrace.h"

using namespace stone;
using namespace stone::file;

CompilationEntity::CompilationEntity(CompilationEntityKind kind,
                                     FileType fileType)
    : kind(kind), fileType(fileType) {

  if (!IsJob()) {
    assert(HasFileType());
  }
}

DriverInputFile *DriverInputFile::Create(const Driver &driver,
                                         llvm::StringRef fileName) {

  // NOTE: The DriverInputFile will attempt to parse the FileType from the name
  return DriverInputFile::Create(driver, fileName, FileType::None);
}

DriverInputFile *Driver::CreateInput(llvm::StringRef fileName) {
  return DriverInputFile::Create(*this, fileName, FileType::None);
}

DriverInputFile *DriverInputFile::Create(const Driver &driver,
                                         llvm::StringRef fileName,
                                         FileType fileType) {
  if (file::IsNoneFileType(fileType)) {
    return new (driver) DriverInputFile(fileName);
  }
  return new (driver) DriverInputFile(fileName, fileType);
}

void CompilationEntityPrettyStackTrace::print(llvm::raw_ostream &OS) const {}
