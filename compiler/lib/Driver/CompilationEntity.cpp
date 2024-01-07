#include "stone/Driver/CompilationEntity.h"
#include "stone/Driver/Driver.h"
#include "stone/Driver/DriverPrettyStackTrace.h"

using namespace stone;
using namespace stone::file;

CompilationEntity::CompilationEntity(CompilationEntityKind kind,
                                     FileType fileType)
    : kind(kind), fileType(fileType) {

  ClearAllFlags();
  ComputeAllFlags(kind);
}

void CompilationEntity::ClearAllFlags() {

  ClearAllowTopLevel();
  ClearAllowFileType();
  ClearAllowOutput();
}

void CompilationEntity::ComputeAllFlags(CompilationEntityKind kind) {

  switch (kind) {
  case CompilationEntityKind::Input: {
    AddAllowFileType();
    break;
  }
  case CompilationEntityKind::CompileJobConstruction: {
    AddAllowTopLevel();
    AddAllowOutput();
    AddAllowFileType();
    break;
  }
  case CompilationEntityKind::BackendJobConstruction: {
    AddAllowTopLevel();
    AddAllowFileType();
    AddAllowOutput();
    break;
  }
  case CompilationEntityKind::GeneratePCHJobConstruction: {
    AddAllowTopLevel();
    AddAllowFileType();

    break;
  }
  case CompilationEntityKind::MergeModuleJobConstruction: {
    AddAllowTopLevel();
    AddAllowOutput();
    AddAllowFileType();
    break;
  }
  case CompilationEntityKind::ModuleWrapJobConstruction: {
    AddAllowTopLevel();
    AddAllowFileType();
    break;
  }
  case CompilationEntityKind::DynamicLinkJobConstruction: {
    AddAllowTopLevel();
    AddAllowOutput();
    AddAllowFileType();
    break;
  }
  case CompilationEntityKind::StaticLinkJobConstruction: {
    AddAllowTopLevel();
    AddAllowOutput();
    AddAllowFileType();
    break;
  }
  case CompilationEntityKind::InterpretJobConstruction: {
    AddAllowTopLevel();
    break;
  }
  case CompilationEntityKind::AutolinkExtractJobConstruction: {
    AddAllowFileType();
    break;
  }
  case CompilationEntityKind::Job: {
    AddAllowTopLevel();
    AddAllowOutput();
    break;
  }
  case CompilationEntityKind::BatchJob: {
    AddAllowTopLevel();
    AddAllowOutput();
    break;
  }
  default: {
    llvm_unreachable("Invalid CompilationEntity!");
  }
  }
}

void CompilationEntity::Print(ColorStream &colorStream) const {}

// CompilationEntities *CompilationEntities::Create(const Driver& driver,
// llvm::ArrayRef<const CompilationEntity*> entities) {

//   unsigned sizeToAlloc =
//       CompilationEntities::totalSizeToAlloc<const
//       CompilationEntity*>(entities.size());
//   void *allocator = driver.Allocate(sizeToAlloc,
//   alignof(CompilationEntities)); return new (allocator)
//   CompilationEntities(llvm::MutableArrayRef<const CompilationEntity*>());
// }

DriverInputFile *DriverInputFile::Create(const Driver &driver,
                                         llvm::StringRef fileName) {

  // NOTE: The DriverInputFile will attempt to parse the FileType from the name
  return DriverInputFile::Create(driver, fileName, FileType::None);
}

DriverInputFile *Driver::CreateInput(llvm::StringRef fileName,
                                     FileType fileType) {
  return DriverInputFile::Create(*this, fileName, fileType);
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
