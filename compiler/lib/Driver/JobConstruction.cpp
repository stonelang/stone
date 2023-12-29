#include "stone/Driver/JobConstruction.h"

using namespace stone;

IncrementatlJobConstruction::IncrementatlJobConstruction(
    JobConstructionKind kind, JobConstructionInputList inputs,
    file::Type fileType)
    : JobConstruction(kind, llvm::None, fileType) {}

CompileJobConstruction::CompileJobConstruction(file::Type outputFileType)
    : IncrementatlJobConstruction(JobConstructionKind::Compile, llvm::None,
                                  outputFileType) {}

CompileJobConstruction::CompileJobConstruction(JobConstructionInput input,
                                               file::Type outputFileType)
    : IncrementatlJobConstruction(JobConstructionKind::Compile, input,
                                  outputFileType) {}

MergeModuleJobConstruction::MergeModuleJobConstruction(
    JobConstructionInputList inputs)
    : IncrementatlJobConstruction(JobConstructionKind::MergeModule, inputs,
                                  file::Type::StoneModule) {}

DynamicLinkJobConstruction::DynamicLinkJobConstruction(
    JobConstructionInputList inputs, LinkMode linkMode, bool withLTO)
    : JobConstruction(JobConstructionKind::DynamicLink, inputs,
                      file::Type::Image),
      linkMode(linkMode), withLTO(withLTO) {

  assert((linkMode != LinkMode::None) && (linkMode != LinkMode::StaticLibrary));
}

StaticLinkJobConstruction::StaticLinkJobConstruction(
    JobConstructionInputList inputs, LinkMode linkMode)
    : JobConstruction(JobConstructionKind::StaticLink, inputs,
                      file::Type::Image),
      linkMode(linkMode) {
  assert(linkMode == LinkMode::StaticLibrary);
}

BackendJobConstruction::BackendJobConstruction(JobConstructionInput input,
                                               file::Type outputFileType,
                                               size_t inputIndex)
    : JobConstruction(JobConstructionKind::Backend, input, outputFileType),
      inputIndex(inputIndex) {}
