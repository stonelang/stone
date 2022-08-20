#include "stone/Driver/CompilationModel.h"
#include "stone/Driver/Driver.h"
#include "stone/Driver/ToolChain.h"

using namespace stone;

Action *
CompilationModel::ConstructCompileAction(ToolChain &tc, action::Input input,
                                         const OutputOptions &outputOpts) {
  Action *action = nullptr;
  auto tool = tc.GetSC();
  action = tc.GetDriver().MakeAction<CompileAction>(*tool, input,
                                                    outputOpts.outputFileType);
  assert(action);
  return action;
}
Action *CompilationModel::ConstructStaticLinkAction(
    ToolChain &tc, action::InputList inputs, const OutputOptions &outputOpts) {

  Action *action = nullptr;
  auto tool = tc.GetLD();
  action = tc.GetDriver().MakeAction<StaticLinkAction>(*tool, inputs);
  assert(action);
  return action;
}

Action *CompilationModel::ConstructExecLinkAction(
    ToolChain &tc, action::InputList inputs, const OutputOptions &outputOpts) {

  Action *action = nullptr;
  auto tool = tc.GetLD();
  return action;
}

Action *CompilationModel::ConstructDynamicLinkAction(
    ToolChain &tc, action::InputList inputs, const OutputOptions &outputOpts) {

  Action *action = nullptr;
  auto ld = tc.GetLD();
  return action;
}

//  TODO: Look into the ActionCache instead of the action::Input
/// GOAL: Build the link action and CacheForTopLevel(..)
Action *CompilationModel::BuildLinkAction(ToolChain &tc, ActionCache &ac,
                                          const OutputOptions &outputOpts) {

  Action *action = nullptr;
  // Make sure that we can link
  assert(tc.GetDriver().CanLink() &&
         "The current mode does not allow linking.");

  // Make sure that there is stuff to link
  assert(ac.HasCompile() && "There is nothing to link");

  switch (tc.GetDriver().GetLinkMode()) {
  case LinkMode::EmitExecutable:
    action = ConstructExecLinkAction(tc, ac.forCompile, outputOpts);
    break;
  case LinkMode::EmitStaticLibrary:
    action = ConstructStaticLinkAction(tc, ac.forCompile, outputOpts);
    break;
  case LinkMode::EmitDynamicLibrary:
    action = ConstructDynamicLinkAction(tc, ac.forCompile, outputOpts);
    break;
  default:
    stone::Panic("Alien link mode");
  }
  assert(action);
  return action;
}

Action *CompilationModel::BuildLinkAction(ToolChain &tc,
                                          const file::Files &inputs,
                                          const OutputOptions &outputOpts) {
  Action *action = nullptr;
  assert(tc.GetDriver().JustLink() && "The current mode is only for linking");
  return nullptr;
}

void QuadraticCompilationModel::BuildCompileActions(
    ToolChain &tc, const file::Files &inputs, ActionCache &ac,
    const OutputOptions &outputOpts) {

  auto BuildCompileAction = [&](const file::File &primaryInput,
                                const file::Files &inputs,
                                const OutputOptions &outputOpts) -> Action * {
    auto action = ConstructCompileAction(
        tc, const_cast<file::File *>(&primaryInput), outputOpts);
    assert(action);
    for (auto &input : inputs) {
      /// The tool chain stores the actions that it created.
      action->AddInput(const_cast<file::File *>(&input));
    }
    return action;
  };
  for (auto &input : inputs) {
    assert(input.GetType() == file::Type::Stone); // Only file-type for now
    auto action = BuildCompileAction(input, inputs, outputOpts);
    ac.CacheForCompile(action);
  }
}
void QuadraticCompilationModel::BuildActions(ToolChain &tc,
                                             const file::Files &inputs,
                                             ActionCache &ac,
                                             const OutputOptions &outputOpts) {

  if (tc.GetDriver().GetDriverOptions().GetMode().CanCompile()) {
    BuildCompileActions(tc, inputs, ac, outputOpts);
    if (tc.GetDriver().JustCompile()) {
      return;
    }
  }
  if (tc.GetDriver().CanLink()) {
    if (tc.GetDriver().JustLink()) {
      auto action = BuildLinkAction(tc, inputs, outputOpts);
    } else {
      // May want to check that you have compile
      auto action = BuildLinkAction(tc, ac, outputOpts);
      ac.CacheForTopLevel(action);
    }
  }
}
void QuadraticCompilationModel::BuildJobs(ToolChain &tc, ActionCache &ac,
                                          JobCache &jc,
                                          const OutputOptions &outputOpts) {

  // auto BuildJobsForAction = [&](const Action *topAction) -> void {
  //   for (auto input : *topAction) {
  //   }
  // };

  // for (auto topInput : ic.forTop) {
  //   BuildJobsForAction(topAction);
  // }
}

// auto BuildCompileTaskDetails = [&]() -> void {

// };

// if(tc.GetDriver().JustCompile()){

// }
// if (tc.GetDriver().GetMode().CanCompile()) {
//   BuildCompileActions(driver, tc, inputs, ic, outputOpts);
//   if (tc.GetDriver().JustCompile()) {
//     return;
//   }
// }
// if (tc.GetDriver().CanLink()) {
//   if (tc.GetDriver().JustLink()) {
//     BuildLinkAction(driver, tc, inputs, outputOpts);
//   } else {
//     BuildLinkAction(driver, tc, ic, outputOpts);
//   }
// }

// // if we have nothing to do, we return
// if (ic.ForCompile()) {
//   return nullptr;
// }
// for (auto input : ic.forCompile) {
//   auto action = InputToAction(input);
//   assert(action);
//   auto taskDetail =
//   tc.ConstructTaskDetail(llvm::cast<CompileAction>(*action));
// }
// }

std::unique_ptr<Compilation> QuadraticCompilationModel::BuildCompilation(
    ToolChain &tc, const file::Files &inputs, const OutputOptions &outputOpts) {

  ActionCache ac;
  BuildActions(tc, inputs, ac, outputOpts);

  JobCache jc;
  BuildJobs(tc, ac, jc, outputOpts);

  // TODO: if print ....

  // TODO: it seems that we can skip these steps if we create the compilation
  // ahead of time and just do
  /// compilation.AddTaskDetail(tc.ConstructTaskDetail(llvm::cast<CompileAction>(*action)))

  // TODO: Check input size
  // Now, build the action system since we have a toolchain
  // auto compilation =
  //     std::make_unique<Compilation>(*this, toolChain, std::move(dal));

  return nullptr;
}

// void FlatCompilationModel::BuildActions(Compilation& compilation, const
// file::Files &inputs,
//                                      ActionCache &ic,
//                                      const OutputOptions &outputOpts) {}

// std::unique_ptr<Compilation>
// FlatCompilationModel::BuildCompilation(Driver &driver,
//                                        const file::Files &inputs, ActionCache
//                                        &ic, const OutputOptions &outputOpts)
//                                        {

//   BuildActions(driver, inputs, ic, outputOpts);
//   return nullptr;
// }

// void CPUCompilationModel::BuildActions(Compilation& compilation, const
// file::Files &inputs,
//                                     ActionCache &ic,
//                                     const OutputOptions &outputOpts) {}

// std::unique_ptr<Compilation>
// CPUCompilationModel::BuildCompilation(Driver &driver, const file::Files
// &inputs,
//                                       ActionCache &ic,
//                                       const OutputOptions &outputOpts) {

//   BuildActions(driver, inputs, ic, outputOpts);
//   return nullptr;
// }

// void SingleCompilationModel::BuildActions(Compilation& compilation,
//                                        const file::Files &inputs, ActionCache
//                                        &ic, const OutputOptions &outputOpts)
//                                        {}

// std::unique_ptr<Compilation> SingleCompilationModel::BuildCompilation(
//     Driver &driver, const file::Files &inputs, ActionCache &ic,
//     const OutputOptions &outputOpts) {

//   BuildActions(driver, inputs, ic, outputOpts);
//   return nullptr;
// }
