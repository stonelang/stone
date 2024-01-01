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

CompilationInputFile *CompilationInputFile::Create(const Driver &driver,
                                         llvm::StringRef fileName,
                                         FileType fileType) {
  if (file::IsNoneFileType(fileType)) {
    return new (driver) CompilationInputFile(fileName);
  }
  return new (driver) CompilationInputFile(fileName, fileType);
}


void CompilationEntityPrettyStackTrace::print(llvm::raw_ostream &OS) const {

}