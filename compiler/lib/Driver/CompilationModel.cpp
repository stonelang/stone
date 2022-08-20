#include "stone/Driver/CompilationModel.h"
#include "stone/Driver/Driver.h"
#include "stone/Driver/ToolChain.h"

using namespace stone;

Phase *
CompilationModel::ConstructCompilePhase(ToolChain &tc, PhaseInput input,
                                         const OutputOptions &outputOpts) {
  Phase *action = nullptr;
  auto tool = tc.GetSC();
  action = tc.GetDriver().MakePhase<CompilePhase>(*tool, input,
                                                    outputOpts.outputFileType);
  assert(action);
  return action;
}
Phase *CompilationModel::ConstructStaticLinkPhase(
    ToolChain &tc, PhaseInputList inputs, const OutputOptions &outputOpts) {

  Phase *action = nullptr;
  auto tool = tc.GetLD();
  action = tc.GetDriver().MakePhase<StaticLinkPhase>(*tool, inputs);
  assert(action);
  return action;
}

Phase *CompilationModel::ConstructExecLinkPhase(
    ToolChain &tc, PhaseInputList inputs, const OutputOptions &outputOpts) {

  Phase *action = nullptr;
  auto tool = tc.GetLD();
  return action;
}

Phase *CompilationModel::ConstructDynamicLinkPhase(
    ToolChain &tc, PhaseInputList inputs, const OutputOptions &outputOpts) {

  Phase *action = nullptr;
  auto ld = tc.GetLD();
  return action;
}

//  TODO: Look into the PhaseCache instead of the PhaseInput
/// GOAL: Build the link action and CacheForTopLevel(..)
Phase *CompilationModel::BuildLinkPhase(ToolChain &tc, PhaseCache &ac,
                                          const OutputOptions &outputOpts) {

  Phase *action = nullptr;
  // Make sure that we can link
  assert(tc.GetDriver().CanLink() &&
         "The current mode does not allow linking.");

  // Make sure that there is stuff to link
  assert(ac.HasCompile() && "There is nothing to link");

  switch (tc.GetDriver().GetLinkMode()) {
  case LinkMode::EmitExecutable:
    action = ConstructExecLinkPhase(tc, ac.forCompile, outputOpts);
    break;
  case LinkMode::EmitStaticLibrary:
    action = ConstructStaticLinkPhase(tc, ac.forCompile, outputOpts);
    break;
  case LinkMode::EmitDynamicLibrary:
    action = ConstructDynamicLinkPhase(tc, ac.forCompile, outputOpts);
    break;
  default:
    stone::Panic("Alien link mode");
  }
  assert(action);
  return action;
}

Phase *CompilationModel::BuildLinkPhase(ToolChain &tc,
                                          const file::Files &inputs,
                                          const OutputOptions &outputOpts) {
  Phase *action = nullptr;
  assert(tc.GetDriver().JustLink() && "The current mode is only for linking");
  return nullptr;
}

void QuadraticCompilationModel::BuildCompilePhases(
    ToolChain &tc, const file::Files &inputs, PhaseCache &ac,
    const OutputOptions &outputOpts) {

  auto BuildCompilePhase = [&](const file::File &primaryInput,
                                const file::Files &inputs,
                                const OutputOptions &outputOpts) -> Phase * {
    auto action = ConstructCompilePhase(
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
    auto action = BuildCompilePhase(input, inputs, outputOpts);
    ac.CacheForCompile(action);
  }
}
void QuadraticCompilationModel::BuildPhases(ToolChain &tc,
                                             const file::Files &inputs,
                                             PhaseCache &ac,
                                             const OutputOptions &outputOpts) {

  if (tc.GetDriver().GetDriverOptions().GetMode().CanCompile()) {
    BuildCompilePhases(tc, inputs, ac, outputOpts);
    if (tc.GetDriver().JustCompile()) {
      return;
    }
  }
  if (tc.GetDriver().CanLink()) {
    if (tc.GetDriver().JustLink()) {
      auto action = BuildLinkPhase(tc, inputs, outputOpts);
    } else {
      // May want to check that you have compile
      auto action = BuildLinkPhase(tc, ac, outputOpts);
      ac.CacheForTopLevel(action);
    }
  }
}
void QuadraticCompilationModel::BuildJobs(ToolChain &tc, PhaseCache &ac,
                                          JobCache &jc,
                                          const OutputOptions &outputOpts) {

  // auto BuildJobsForPhase = [&](const Phase *topPhase) -> void {
  //   for (auto input : *topPhase) {
  //   }
  // };

  // for (auto topInput : ic.forTop) {
  //   BuildJobsForPhase(topPhase);
  // }
}

// auto BuildCompileTaskDetails = [&]() -> void {

// };

// if(tc.GetDriver().JustCompile()){

// }
// if (tc.GetDriver().GetMode().CanCompile()) {
//   BuildCompilePhases(driver, tc, inputs, ic, outputOpts);
//   if (tc.GetDriver().JustCompile()) {
//     return;
//   }
// }
// if (tc.GetDriver().CanLink()) {
//   if (tc.GetDriver().JustLink()) {
//     BuildLinkPhase(driver, tc, inputs, outputOpts);
//   } else {
//     BuildLinkPhase(driver, tc, ic, outputOpts);
//   }
// }

// // if we have nothing to do, we return
// if (ic.ForCompile()) {
//   return nullptr;
// }
// for (auto input : ic.forCompile) {
//   auto action = InputToPhase(input);
//   assert(action);
//   auto taskDetail =
//   tc.ConstructTaskDetail(llvm::cast<CompilePhase>(*action));
// }
// }

std::unique_ptr<Compilation> QuadraticCompilationModel::BuildCompilation(
    ToolChain &tc, const file::Files &inputs, const OutputOptions &outputOpts) {

  PhaseCache ac;
  BuildPhases(tc, inputs, ac, outputOpts);

  JobCache jc;
  BuildJobs(tc, ac, jc, outputOpts);

  // TODO: if print ....

  // TODO: it seems that we can skip these steps if we create the compilation
  // ahead of time and just do
  /// compilation.AddTaskDetail(tc.ConstructTaskDetail(llvm::cast<CompilePhase>(*action)))

  // TODO: Check input size
  // Now, build the action system since we have a toolchain
  // auto compilation =
  //     std::make_unique<Compilation>(*this, toolChain, std::move(dal));

  return nullptr;
}

// void FlatCompilationModel::BuildPhases(Compilation& compilation, const
// file::Files &inputs,
//                                      PhaseCache &ic,
//                                      const OutputOptions &outputOpts) {}

// std::unique_ptr<Compilation>
// FlatCompilationModel::BuildCompilation(Driver &driver,
//                                        const file::Files &inputs, PhaseCache
//                                        &ic, const OutputOptions &outputOpts)
//                                        {

//   BuildPhases(driver, inputs, ic, outputOpts);
//   return nullptr;
// }

// void CPUCompilationModel::BuildPhases(Compilation& compilation, const
// file::Files &inputs,
//                                     PhaseCache &ic,
//                                     const OutputOptions &outputOpts) {}

// std::unique_ptr<Compilation>
// CPUCompilationModel::BuildCompilation(Driver &driver, const file::Files
// &inputs,
//                                       PhaseCache &ic,
//                                       const OutputOptions &outputOpts) {

//   BuildPhases(driver, inputs, ic, outputOpts);
//   return nullptr;
// }

// void SingleCompilationModel::BuildPhases(Compilation& compilation,
//                                        const file::Files &inputs, PhaseCache
//                                        &ic, const OutputOptions &outputOpts)
//                                        {}

// std::unique_ptr<Compilation> SingleCompilationModel::BuildCompilation(
//     Driver &driver, const file::Files &inputs, PhaseCache &ic,
//     const OutputOptions &outputOpts) {

//   BuildPhases(driver, inputs, ic, outputOpts);
//   return nullptr;
// }
