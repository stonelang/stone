#include "stone/Driver/JobConstruction.h"

using namespace stone;
using namespace stone::file;

IncrementatlJobConstruction::IncrementatlJobConstruction(
    JobConstructionKind kind, JobConstructionInputList inputs,
    FileType fileType)
    : JobConstruction(kind, llvm::None, fileType) {}

llvm::ArrayRef<const Job *> IncrementatlJobConstruction::ConstructJobs() {

  return {nullptr};
}

CompileJobConstruction::CompileJobConstruction(FileType outputFileType)
    : IncrementatlJobConstruction(JobConstructionKind::Compile, llvm::None,
                                  outputFileType) {

  assert(file::IsOutputableFileType(outputFileType));
}

llvm::ArrayRef<const Job *> CompileJobConstruction::ConstructJobs() {

  if (!HasTopLevel()) {
    return {nullptr};
  }
}

CompileJobConstruction *
CompileJobConstruction::Create(Driver &driver, FileType outputFileType) {

  return new (driver) CompileJobConstruction(outputFileType);
}

CompileJobConstruction::CompileJobConstruction(JobConstructionInput input,
                                               FileType outputFileType)
    : IncrementatlJobConstruction(JobConstructionKind::Compile, input,
                                  outputFileType) {

  assert(file::IsOutputableFileType(outputFileType));
}

CompileJobConstruction *
CompileJobConstruction::Create(Driver &driver, JobConstructionInput input,
                               FileType outputFileType) {

  return new (driver) CompileJobConstruction(input, outputFileType);
}

MergeModuleJobConstruction::MergeModuleJobConstruction(
    JobConstructionInputList inputs)
    : IncrementatlJobConstruction(JobConstructionKind::MergeModule, inputs,
                                  FileType::StoneModule) {}

llvm::ArrayRef<const Job *> MergeModuleJobConstruction::ConstructJobs() {

  return {nullptr};
}

LinkJobConstruction::LinkJobConstruction(JobConstructionKind kind,
                                         JobConstructionInputList inputs,
                                         FileType outputFileType,
                                         LinkMode linkMode)
    : JobConstruction(kind, inputs, outputFileType), linkMode(linkMode) {}

llvm::ArrayRef<const Job *> LinkJobConstruction::ConstructJobs() {

  return {nullptr};
}

DynamicLinkJobConstruction::DynamicLinkJobConstruction(
    JobConstructionInputList inputs, LinkMode linkMode, bool withLTO)
    : LinkJobConstruction(JobConstructionKind::DynamicLink, inputs,
                          FileType::Image, linkMode),
      withLTO(withLTO) {

  assert((linkMode != LinkMode::None) && (linkMode != LinkMode::StaticLibrary));
}

DynamicLinkJobConstruction *
DynamicLinkJobConstruction::Create(Driver &driver,
                                   JobConstructionInputList inputs,
                                   LinkMode linkMode, bool withLTO) {

  return new (driver) DynamicLinkJobConstruction(inputs, linkMode, withLTO);
}

StaticLinkJobConstruction::StaticLinkJobConstruction(
    JobConstructionInputList inputs, LinkMode linkMode)
    : LinkJobConstruction(JobConstructionKind::StaticLink, inputs,
                          FileType::Image, linkMode) {
  assert(linkMode == LinkMode::StaticLibrary);
}

StaticLinkJobConstruction *StaticLinkJobConstruction::Create(
    Driver &driver, JobConstructionInputList inputs, LinkMode linkMode) {

  return new (driver) StaticLinkJobConstruction(inputs, linkMode);
}

BackendJobConstruction::BackendJobConstruction(JobConstructionInput input,
                                               FileType outputFileType,
                                               size_t inputIndex)
    : JobConstruction(JobConstructionKind::Backend, input, outputFileType),
      inputIndex(inputIndex) {

  assert(file::IsOutputableFileType(outputFileType));
}

llvm::ArrayRef<const Job *> BackendJobConstruction::ConstructJobs() {

  return {nullptr};
}

GeneratePCHJobConstruction::GeneratePCHJobConstruction(
    JobConstructionInput input, llvm::StringRef persistentPCHDir)
    :

      JobConstruction(JobConstructionKind::GeneratePCH, input,
                      persistentPCHDir.empty() ? FileType::PCH
                                               : FileType::None),
      persistentPCHDir(persistentPCHDir) {}

llvm::ArrayRef<const Job *> GeneratePCHJobConstruction::ConstructJobs() {

  return {nullptr};
}

void CompilationEntities::AddTopLevelJobConstruction(
    const JobConstruction *construction) {
  assert(construction);
  assert(construction->HasTopLevel());
  topLevelJobConstructions.push_back(construction);
}

void CompilationEntities::ForEachTopLevelJobConstruction(
    std::function<void(const JobConstruction *construction)> callback) {
  for (auto topLevelJobConstruction : topLevelJobConstructions) {
    callback(topLevelJobConstruction);
  }
}
