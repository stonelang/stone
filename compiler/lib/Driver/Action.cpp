#include "stone/Driver/Action.h"
#include "stone/Driver/Driver.h"
using namespace stone;

InputAction::InputAction(InputFile input)
    : Action(ActionKind::Input, input.GetFileType()), input(input) {}

InputAction *InputAction::Create(Driver &driver, InputFile input) {
  return new (driver) InputAction(input);
}

JobAction::JobAction(ActionKind kind, Action *input, FileType fileType)
    : Action(kind, input, fileType) {}

JobAction::JobAction(ActionKind kind, const ActionList &inputs,
                     FileType fileType)
    : Action(kind, inputs, fileType) {}

CompileJobAction::CompileJobAction(FileType outputFileType)
    : JobAction(ActionKind::CompileJob, ActionList(), outputFileType) {}

CompileJobAction::CompileJobAction(Action *input, FileType outputFileType)
    : JobAction(ActionKind::CompileJob, input, outputFileType) {}

CompileJobAction *CompileJobAction::Create(Driver &driver,
                                           FileType outputFileType) {

  return new (driver) CompileJobAction(outputFileType);
}

CompileJobAction *CompileJobAction::Create(Driver &driver, Action *input,
                                           FileType outputFileType) {

  return new (driver) CompileJobAction(input, outputFileType);
}

BackendJobAction::BackendJobAction(Action *input, FileType outputFileType,
                                   size_t inputIndex)
    : JobAction(ActionKind::BackendJob, input, outputFileType),
      inputIndex(inputIndex) {}

BackendJobAction *BackendJobAction::Create(Driver &driver, Action *input,
                                           FileType outputFileType,
                                           size_t inputIndex) {

  return new (driver) BackendJobAction(input, outputFileType, inputIndex);
}

MergeModuleJobAction::MergeModuleJobAction(ActionList inputs)
    : JobAction(ActionKind::MergeModuleJob, inputs, FileType::StoneModuleFile) {
}

MergeModuleJobAction *MergeModuleJobAction::Create(Driver &driver,
                                                   ActionList inputs) {

  return new (driver) MergeModuleJobAction(inputs);
}

StaticLinkJobAction::StaticLinkJobAction(ActionList inputs, LinkMode linkMode)
    : JobAction(ActionKind::StaticLinkJob, inputs, FileType::Image) {
  assert(linkMode == LinkMode::StaticLibrary);
}

StaticLinkJobAction *StaticLinkJobAction::Create(Driver &driver,
                                                 ActionList inputs,
                                                 LinkMode linkMode) {

  return new (driver) StaticLinkJobAction(inputs, linkMode);
}

DynamicLinkJobAction::DynamicLinkJobAction(ActionList inputs, LinkMode linkMode,
                                           bool withLTO)
    : JobAction(ActionKind::DynamicLinkJob, inputs, FileType::Image),
      linkMode(linkMode), withLTO(withLTO) {

  assert((linkMode != LinkMode::None) && (linkMode != LinkMode::StaticLibrary));
}

DynamicLinkJobAction *DynamicLinkJobAction::Create(Driver &driver,
                                                   ActionList inputs,
                                                   LinkMode linkMode,
                                                   bool withLTO) {

  return new (driver) DynamicLinkJobAction(inputs, linkMode, withLTO);
}