#include "stone/Driver/CompilationEntity.h"
#include "stone/Driver/Driver.h"
#include "stone/Driver/DriverInputFile.h"

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
                                         llvm::StringRef fileName,
                                         FileType fileType) {
  if (file::IsNoneFileType(fileType)) {
    return new (driver) DriverInputFile(fileName);
  }
  return new (driver) DriverInputFile(fileName, fileType);
}
