#include "stone/Driver/JobConstruction.h"

using namespace stone;
using namespace stone::file;

IncrementatlJobConstruction::IncrementatlJobConstruction(
    JobConstructionKind kind, JobConstructionInputList inputs,
    FileType fileType)
    : JobConstruction(kind, llvm::None, fileType) {}

CompileJobConstruction::CompileJobConstruction(FileType outputFileType)
    : IncrementatlJobConstruction(JobConstructionKind::Compile, llvm::None,
                                  outputFileType) {

  assert(file::IsOutputableFileType(outputFileType));
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

LinkJobConstruction::LinkJobConstruction(JobConstructionKind kind,
                                         JobConstructionInputList inputs,
                                         FileType outputFileType,
                                         LinkMode linkMode)
    : JobConstruction(kind, inputs, outputFileType), linkMode(linkMode) {}

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

GeneratePCHJobConstruction::GeneratePCHJobConstruction(
    JobConstructionInput input, llvm::StringRef persistentPCHDir)
    :

      JobConstruction(JobConstructionKind::GeneratePCH, input,
                      persistentPCHDir.empty() ? FileType::PCH
                                               : FileType::None),
      persistentPCHDir(persistentPCHDir) {}
