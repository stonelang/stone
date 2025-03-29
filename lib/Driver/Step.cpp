#include "stone/Driver/Step.h"
#include "stone/Driver/Driver.h"
using namespace stone;

Step::Step(StepKind kind, FileType fileType) : Step(kind, Steps(), fileType) {}

Step::Step(StepKind kind, Step *input, FileType fileType)
    : Step(kind, Steps({input}), fileType) {}

Step::Step(StepKind kind, Step *input)
    : Step(kind, Steps({input}), input->GetFileType()) {}

Step::Step(StepKind kind, const Steps &inputs, FileType fileType)
    : kind(kind), fileType(fileType), allInputs(inputs) {}

bool Step::IsJobStep(StepKind kind) {
  switch (kind) {
  case StepKind::Compile:
  case StepKind::Backend:
  case StepKind::GeneratePCH:
  case StepKind::MergeModule:
  case StepKind::ModuleWrap:
  case StepKind::Link:
  case StepKind::Interpret:
  case StepKind::AutolinkExtract:
    return true;
  case StepKind::Input:
    return false;
  }
  llvm_unreachable("Unknown step");
}

InputStep::InputStep(InputFile input)
    : Step(StepKind::Input, input.GetFileType()), input(input) {}

InputStep *InputStep::Create(Driver &driver, InputFile input) {
  return new (driver) InputStep(input);
}

JobStep::JobStep(StepKind kind, Step *input, FileType fileType)
    : Step(kind, input, fileType) {}

JobStep::JobStep(StepKind kind, const Steps &inputs, FileType fileType)
    : Step(kind, inputs, fileType) {}

CompileStep::CompileStep(FileType outputFileType)
    : JobStep(StepKind::Compile, Steps(), outputFileType) {}

CompileStep::CompileStep(Step *input, FileType outputFileType)
    : JobStep(StepKind::Compile, input, outputFileType) {}

CompileStep *CompileStep::Create(Driver &driver, FileType outputFileType) {

  return new (driver) CompileStep(outputFileType);
}

CompileStep *CompileStep::Create(Driver &driver, Step *input,
                                 FileType outputFileType) {

  return new (driver) CompileStep(input, outputFileType);
}

BackendStep::BackendStep(Step *input, FileType outputFileType,
                         size_t inputIndex)
    : JobStep(StepKind::Backend, input, outputFileType),
      inputIndex(inputIndex) {}

BackendStep *BackendStep::Create(Driver &driver, Step *input,
                                 FileType outputFileType, size_t inputIndex) {

  return new (driver) BackendStep(input, outputFileType, inputIndex);
}

MergeModuleStep::MergeModuleStep(Steps inputs)
    : JobStep(StepKind::MergeModule, inputs, FileType::StoneModuleFile) {}

MergeModuleStep *MergeModuleStep::Create(Driver &driver, Steps inputs) {

  return new (driver) MergeModuleStep(inputs);
}

LinkStep::LinkStep(Steps inputs, LinkType linkType, bool allowLTO)
    : JobStep(StepKind::Link, inputs, FileType::Image), linkType(linkType),
      allowLTO(allowLTO) {

  assert((linkType != LinkType::None));
  assert((linkType == LinkType::StaticLibrary) && !allowLTO);
}

LinkStep *LinkStep::Create(Driver &driver, Steps inputs, LinkType linkType,
                           bool withLTO) {

  return new (driver) LinkStep(inputs, linkType, withLTO);
}
