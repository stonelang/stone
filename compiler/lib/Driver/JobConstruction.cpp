#include "stone/Driver/JobConstruction.h"
#include "stone/Basic/FileType.h"
#include "stone/Driver/Compilation.h"
#include "stone/Driver/Driver.h"
#include "stone/Driver/DriverPrettyStackTrace.h"

using namespace stone;
using namespace stone::file;

JobConstruction::JobConstruction(CompilationEntityKind kind,
                                 CompilationEntityList inputs,
                                 file::FileType fileType)
    : TopLevelCompilationEntity(kind, inputs, fileType) {

  /// create the Job
  // You have the inputs, so you can add them : job->AddInput()
}

CompileJobConstruction::CompileJobConstruction(FileType outputFileType)
    : IncrementalJobConstruction(CompilationEntityKind::CompileJobConstruction,
                                 llvm::None, outputFileType) {

  assert(file::IsOutputFileType(outputFileType));
}

CompileJobConstruction::CompileJobConstruction(const CompilationEntity *input,
                                               FileType outputFileType)
    : IncrementalJobConstruction(CompilationEntityKind::CompileJobConstruction,
                                 input, outputFileType) {

  assert(file::IsOutputFileType(outputFileType));
}

CompileJobConstruction *
CompileJobConstruction::Create(const Driver &driver,
                               const CompilationEntity *input,
                               FileType outputFileType) {
  return new (driver) CompileJobConstruction(input, outputFileType);
}

CompileJobConstruction *
CompileJobConstruction::Create(const Driver &driver, FileType outputFileType) {
  return new (driver) CompileJobConstruction(outputFileType);
}

MergeModuleJobConstruction::MergeModuleJobConstruction(
    CompilationEntityList inputs)
    : IncrementalJobConstruction(
          CompilationEntityKind::MergeModuleJobConstruction, inputs,
          file::FileType::StoneModule) {}

MergeModuleJobConstruction *
MergeModuleJobConstruction::Create(const Driver &driver,
                                   CompilationEntityList inputs) {
  return new (driver) MergeModuleJobConstruction(inputs);
}

ModuleWrapJobConstruction::ModuleWrapJobConstruction(
    CompilationEntityList inputs)
    : JobConstruction(CompilationEntityKind::ModuleWrapJobConstruction, inputs,
                      FileType::Object) {}

ModuleWrapJobConstruction *
ModuleWrapJobConstruction::Create(const Driver &driver,
                                  CompilationEntityList inputs) {
  return new (driver) ModuleWrapJobConstruction(inputs);
}

DynamicLinkJobConstruction::DynamicLinkJobConstruction(
    CompilationEntityList inputs, LinkMode linkMode, bool withLTO)
    : LinkJobConstruction(CompilationEntityKind::DynamicLinkJobConstruction,
                          inputs, FileType::Image, linkMode),
      withLTO(withLTO) {

  assert((linkMode != LinkMode::None) && (linkMode != LinkMode::StaticLibrary));
}

DynamicLinkJobConstruction *
DynamicLinkJobConstruction::Create(Driver &driver, CompilationEntityList inputs,
                                   LinkMode linkMode, bool withLTO) {

  return new (driver) DynamicLinkJobConstruction(inputs, linkMode, withLTO);
}

StaticLinkJobConstruction::StaticLinkJobConstruction(
    CompilationEntityList inputs, LinkMode linkMode)
    : LinkJobConstruction(CompilationEntityKind::StaticLinkJobConstruction,
                          inputs, FileType::Image, linkMode) {
  assert(linkMode == LinkMode::StaticLibrary);
}

StaticLinkJobConstruction *
StaticLinkJobConstruction::Create(Driver &driver, CompilationEntityList inputs,
                                  LinkMode linkMode) {
  return new (driver) StaticLinkJobConstruction(inputs, linkMode);
}

BackendJobConstruction::BackendJobConstruction(const CompilationEntity *input,
                                               FileType outputFileType,
                                               size_t inputIndex)
    : JobConstruction(CompilationEntityKind::BackendJobConstruction, input,
                      outputFileType),
      inputIndex(inputIndex) {

  assert(file::IsOutputFileType(outputFileType));
}

GeneratePCHJobConstruction::GeneratePCHJobConstruction(
    const CompilationEntity *input, llvm::StringRef persistentPCHDir)
    :

      JobConstruction(CompilationEntityKind::GeneratePCHJobConstruction, input,
                      persistentPCHDir.empty() ? FileType::PCH
                                               : FileType::None),
      persistentPCHDir(persistentPCHDir) {}

GeneratePCHJobConstruction *
GeneratePCHJobConstruction::Create(Driver &driver,
                                   const CompilationEntity *input,
                                   llvm::StringRef persistentPCHDir) {
  return new (driver) GeneratePCHJobConstruction(input, persistentPCHDir);
}
