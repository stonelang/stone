#include "stone/Driver/Step.h"
#include "stone/Driver/Driver.h"
using namespace stone;

InputStep::InputStep(InputFile input)
    : Step(StepKind::Input, input.GetFileType()), input(input) {}

InputStep *InputStep::Create(Driver &driver, InputFile input) {
  return new (driver) InputStep(input);
}

Step::Step(StepKind kind, FileType fileType) : Step(kind, Steps(), fileType) {}

Step::Step(StepKind kind, Step *input, FileType fileType)
    : Step(kind, Steps({input}), fileType) {}

Step::Step(StepKind kind, Step *input)
    : Step(kind, Steps({input}), input->GetFileType()) {}

Step::Step(StepKind kind, const Steps &inputs, FileType fileType)
    : kind(kind), fileType(fileType), allInputs(inputs) {}

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

StaticLinkStep::StaticLinkStep(Steps inputs, LinkMode linkMode)
    : JobStep(StepKind::StaticLink, inputs, FileType::Image) {
  assert(linkMode == LinkMode::StaticLibrary);
}

StaticLinkStep *StaticLinkStep::Create(Driver &driver, Steps inputs,
                                       LinkMode linkMode) {

  return new (driver) StaticLinkStep(inputs, linkMode);
}

DynamicLinkStep::DynamicLinkStep(Steps inputs, LinkMode linkMode, bool withLTO)
    : JobStep(StepKind::DynamicLink, inputs, FileType::Image),
      linkMode(linkMode), withLTO(withLTO) {

  assert((linkMode != LinkMode::None) && (linkMode != LinkMode::StaticLibrary));
}

DynamicLinkStep *DynamicLinkStep::Create(Driver &driver, Steps inputs,
                                         LinkMode linkMode, bool withLTO) {

  return new (driver) DynamicLinkStep(inputs, linkMode, withLTO);
}