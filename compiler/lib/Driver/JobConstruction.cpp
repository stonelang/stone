#include "stone/Driver/JobConstruction.h"

using namespace stone;

CompileJobConstruction::CompileJobConstruction(file::Type outputFileType)
    : JobConstruction(JobConstructionKind::Compile, llvm::None,
                      outputFileType) {}

CompileJobConstruction::CompileJobConstruction(JobConstructionInput input,
                                               file::Type outputFileType)
    : JobConstruction(JobConstructionKind::Compile, input, outputFileType) {}

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

BackendJobConstruction::BackendJobConstruction(
    JobConstructionInput input, file::Type outputFileType, size_t inputIndex)
    : JobConstruction(JobConstructionKind::Backend, input, outputFileType),
      inputIndex(inputIndex) {}
