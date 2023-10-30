#include "stone/Drive/CompilationModel.h"
#include "stone/Drive/Driver.h"
#include "stone/Drive/ToolChain.h"

using namespace stone;

JobAction *
CompilationModel::ConstructCompileJobAction(ToolChain &tc, JobActionInput input,
                                        const OutputOptions &outputOpts) {
  JobAction *phase = nullptr;
  auto tool = tc.GetSC();
  phase = tc.GetDriver().MakeJobAction<CompileJobAction>(*tool, input,
                                                 outputOpts.outputFileType);
  assert(phase);
  return phase;
}
JobAction *
CompilationModel::ConstructStaticLinkJobAction(ToolChain &tc, JobActionInputList inputs,
                                           const OutputOptions &outputOpts) {

  JobAction *phase = nullptr;
  auto tool = tc.GetLD();
  phase = tc.GetDriver().MakeJobAction<StaticLinkJobAction>(*tool, inputs);
  assert(phase);
  return phase;
}

JobAction *
CompilationModel::ConstructExecLinkJobAction(ToolChain &tc, JobActionInputList inputs,
                                         const OutputOptions &outputOpts) {

  JobAction *phase = nullptr;
  auto tool = tc.GetLD();
  return phase;
}

JobAction *CompilationModel::ConstructDynamicLinkJobAction(
    ToolChain &tc, JobActionInputList inputs, const OutputOptions &outputOpts) {

  JobAction *phase = nullptr;
  auto ld = tc.GetLD();
  return phase;
}

//  TODO: Look into the JobActionCache instead of the JobActionInput
/// GOAL: Build the link phase and CacheForTopLevel(..)
JobAction *CompilationModel::BuildLinkJobAction(ToolChain &tc, JobActionCache &ac,
                                        const OutputOptions &outputOpts) {

  JobAction *phase = nullptr;
  // Make sure that we can link
  assert(tc.GetDriver().CanLink() &&
         "The current mode does not allow linking.");

  // Make sure that there is stuff to link
  assert(ac.HasCompile() && "There is nothing to link");

  switch (tc.GetDriver().GetLinkMode()) {
  case LinkMode::EmitExecutable:
    phase = ConstructExecLinkJobAction(tc, ac.forCompile, outputOpts);
    break;
  case LinkMode::EmitStaticLibrary:
    phase = ConstructStaticLinkJobAction(tc, ac.forCompile, outputOpts);
    break;
  case LinkMode::EmitDynamicLibrary:
    phase = ConstructDynamicLinkJobAction(tc, ac.forCompile, outputOpts);
    break;
  default:
    stone::Panic("Alien link mode");
  }
  assert(phase);
  return phase;
}

JobAction *CompilationModel::BuildLinkJobAction(ToolChain &tc,
                                        const file::Files &inputs,
                                        const OutputOptions &outputOpts) {
  JobAction *phase = nullptr;
  assert(tc.GetDriver().JustLink() && "The current mode is only for linking");
  return nullptr;
}

void QuadraticCompilationModel::BuildCompileJobActions(
    ToolChain &tc, const file::Files &inputs, JobActionCache &ac,
    const OutputOptions &outputOpts) {

  auto BuildCompileJobAction = [&](const file::File &primaryInput,
                               const file::Files &inputs,
                               const OutputOptions &outputOpts) -> JobAction * {
    auto phase = ConstructCompileJobAction(
        tc, const_cast<file::File *>(&primaryInput), outputOpts);
    assert(phase);
    for (auto &input : inputs) {
      /// The tool chain stores the phases that it created.
      phase->AddInput(const_cast<file::File *>(&input));
    }
    return phase;
  };
  for (auto &input : inputs) {
    assert(input.GetType() == file::Type::Stone); // Only file-type for now
    auto phase = BuildCompileJobAction(input, inputs, outputOpts);
    ac.CacheForCompile(phase);
  }
}
void QuadraticCompilationModel::BuildJobActions(ToolChain &tc,
                                            const file::Files &inputs,
                                            JobActionCache &pc,
                                            const OutputOptions &outputOpts) {

  if (tc.GetDriver().CanCompile()) {
    BuildCompileJobActions(tc, inputs, pc, outputOpts);
    if (tc.GetDriver().JustCompile()) {
      pc.forTopLevel.append(pc.forCompile.begin(), pc.forCompile.end());
      return;
    }
  }
  // TODO:  We will get back to this part -- just get compile to work.
  //  if (tc.GetDriver().CanLink()) {
  //    if (tc.GetDriver().JustLink()) {
  //      auto phase = BuildLinkJobAction(tc, inputs, outputOpts);
  //    } else {
  //      // May want to check that you have compile
  //      auto phase = BuildLinkJobAction(tc, pc, outputOpts);
  //      ac.CacheForTopLevel(phase);
  //    }
  //  }

  // if (tc.GetDriver().CanLink()) {
  //     auto linkJobAction = BuildLinkJobAction(tc, pc, outputOpts);
  //     ac.CacheForTopLevel(phase);
  //   }
  // }
}
void QuadraticCompilationModel::BuildJobs(ToolChain &tc, JobActionCache &pc,
                                          JobCache &jc,
                                          const OutputOptions &outputOpts) {

  if (tc.GetDriver().CanCompile()) {
    assert(pc.HasCompile());
    BuildCompileJobs(tc, pc, jc, outputOpts);
  }
}

void QuadraticCompilationModel::BuildCompileJobs(
    ToolChain &tc, JobActionCache &pc, JobCache &jc,
    const OutputOptions &outputOpts) {

  auto BuildCompileJob = [&](ToolChain &tc, const JobAction &phase,
                             const OutputOptions &outputOpts) -> Job * {

  };
  for (auto input : pc.forCompile) {
    jc.CacheForCompile(BuildCompileJob(tc, *InputToJobAction(input), outputOpts));
  }
}

// TODO: This may actually be generic
void QuadraticCompilationModel::BuildJobs(ToolChain &tc, const JobAction &phase,
                                          JobCache &jc,
                                          const OutputOptions &outputOpts) {}

// auto BuildCompileTaskDetails = [&]() -> void {

// };

// if(tc.GetDriver().JustCompile()){

// }
// if (tc.GetDriver().GetMode().CanCompile()) {
//   BuildCompileJobActions(driver, tc, inputs, ic, outputOpts);
//   if (tc.GetDriver().JustCompile()) {
//     return;
//   }
// }
// if (tc.GetDriver().CanLink()) {
//   if (tc.GetDriver().JustLink()) {
//     BuildLinkJobAction(driver, tc, inputs, outputOpts);
//   } else {
//     BuildLinkJobAction(driver, tc, ic, outputOpts);
//   }
// }

// // if we have nothing to do, we return
// if (ic.ForCompile()) {
//   return nullptr;
// }
// for (auto input : ic.forCompile) {
//   auto phase = InputToJobAction(input);
//   assert(phase);
//   auto taskDetail =
//   tc.ConstructTaskDetail(llvm::cast<CompileJobAction>(*phase));
// }
// }

std::unique_ptr<Compilation> QuadraticCompilationModel::BuildCompilation(
    ToolChain &tc, const file::Files &inputs, const OutputOptions &outputOpts) {

  JobActionCache pc;
  BuildJobActions(tc, inputs, pc, outputOpts);

  JobCache jc;
  BuildJobs(tc, pc, jc, outputOpts);

  // TODO: if print ....

  // TODO: it seems that we can skip these steps if we create the compilation
  // ahead of time and just do
  /// compilation.AddTaskDetail(tc.ConstructTaskDetail(llvm::cast<CompileJobAction>(*phase)))

  // TODO: Check input size
  // Now, build the phase system since we have a toolchain
  // auto compilation =
  //     std::make_unique<Compilation>(*this, toolChain, std::move(dal));

  return nullptr;
}

// void FlatCompilationModel::BuildJobActions(Compilation& compilation, const
// file::Files &inputs,
//                                      JobActionCache &ic,
//                                      const OutputOptions &outputOpts) {}

// std::unique_ptr<Compilation>
// FlatCompilationModel::BuildCompilation(Driver &driver,
//                                        const file::Files &inputs, JobActionCache
//                                        &ic, const OutputOptions &outputOpts)
//                                        {

//   BuildJobActions(driver, inputs, ic, outputOpts);
//   return nullptr;
// }

// void CPUCompilationModel::BuildJobActions(Compilation& compilation, const
// file::Files &inputs,
//                                     JobActionCache &ic,
//                                     const OutputOptions &outputOpts) {}

// std::unique_ptr<Compilation>
// CPUCompilationModel::BuildCompilation(Driver &driver, const file::Files
// &inputs,
//                                       JobActionCache &ic,
//                                       const OutputOptions &outputOpts) {

//   BuildJobActions(driver, inputs, ic, outputOpts);
//   return nullptr;
// }

// void SingleCompilationModel::BuildJobActions(Compilation& compilation,
//                                        const file::Files &inputs, JobActionCache
//                                        &ic, const OutputOptions &outputOpts)
//                                        {}

// std::unique_ptr<Compilation> SingleCompilationModel::BuildCompilation(
//     Driver &driver, const file::Files &inputs, JobActionCache &ic,
//     const OutputOptions &outputOpts) {

//   BuildJobActions(driver, inputs, ic, outputOpts);
//   return nullptr;
// }
