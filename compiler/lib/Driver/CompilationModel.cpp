#include "stone/Driver/CompilationModel.h"
#include "stone/Driver/Driver.h"
#include "stone/Driver/ToolChain.h"

using namespace stone;

Phase *
CompilationModel::ConstructCompilePhase(ToolChain &tc, PhaseInput input,
                                        const OutputOptions &outputOpts) {
  Phase *phase = nullptr;
  auto tool = tc.GetSC();
  phase = tc.GetDriver().MakePhase<CompilePhase>(*tool, input,
                                                 outputOpts.outputFileType);
  assert(phase);
  return phase;
}
Phase *
CompilationModel::ConstructStaticLinkPhase(ToolChain &tc, PhaseInputList inputs,
                                           const OutputOptions &outputOpts) {

  Phase *phase = nullptr;
  auto tool = tc.GetLD();
  phase = tc.GetDriver().MakePhase<StaticLinkPhase>(*tool, inputs);
  assert(phase);
  return phase;
}

Phase *
CompilationModel::ConstructExecLinkPhase(ToolChain &tc, PhaseInputList inputs,
                                         const OutputOptions &outputOpts) {

  Phase *phase = nullptr;
  auto tool = tc.GetLD();
  return phase;
}

Phase *CompilationModel::ConstructDynamicLinkPhase(
    ToolChain &tc, PhaseInputList inputs, const OutputOptions &outputOpts) {

  Phase *phase = nullptr;
  auto ld = tc.GetLD();
  return phase;
}

//  TODO: Look into the PhaseCache instead of the PhaseInput
/// GOAL: Build the link phase and CacheForTopLevel(..)
Phase *CompilationModel::BuildLinkPhase(ToolChain &tc, PhaseCache &ac,
                                        const OutputOptions &outputOpts) {

  Phase *phase = nullptr;
  // Make sure that we can link
  assert(tc.GetDriver().CanLink() &&
         "The current mode does not allow linking.");

  // Make sure that there is stuff to link
  assert(ac.HasCompile() && "There is nothing to link");

  switch (tc.GetDriver().GetLinkMode()) {
  case LinkMode::EmitExecutable:
    phase = ConstructExecLinkPhase(tc, ac.forCompile, outputOpts);
    break;
  case LinkMode::EmitStaticLibrary:
    phase = ConstructStaticLinkPhase(tc, ac.forCompile, outputOpts);
    break;
  case LinkMode::EmitDynamicLibrary:
    phase = ConstructDynamicLinkPhase(tc, ac.forCompile, outputOpts);
    break;
  default:
    stone::Panic("Alien link mode");
  }
  assert(phase);
  return phase;
}

Phase *CompilationModel::BuildLinkPhase(ToolChain &tc,
                                        const file::Files &inputs,
                                        const OutputOptions &outputOpts) {
  Phase *phase = nullptr;
  assert(tc.GetDriver().JustLink() && "The current mode is only for linking");
  return nullptr;
}

void QuadraticCompilationModel::BuildCompilePhases(
    ToolChain &tc, const file::Files &inputs, PhaseCache &ac,
    const OutputOptions &outputOpts) {

  auto BuildCompilePhase = [&](const file::File &primaryInput,
                               const file::Files &inputs,
                               const OutputOptions &outputOpts) -> Phase * {
    auto phase = ConstructCompilePhase(
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
    auto phase = BuildCompilePhase(input, inputs, outputOpts);
    ac.CacheForCompile(phase);
  }
}
void QuadraticCompilationModel::BuildPhases(ToolChain &tc,
                                            const file::Files &inputs,
                                            PhaseCache &pc,
                                            const OutputOptions &outputOpts) {

  if (tc.GetDriver().CanCompile()) {
    BuildCompilePhases(tc, inputs, pc, outputOpts);
    if (tc.GetDriver().JustCompile()) {
      pc.forTopLevel.append(pc.forCompile.begin(), pc.forCompile.end());
      return;
    }
  }
  // TODO:  We will get back to this part -- just get compile to work.
  //  if (tc.GetDriver().CanLink()) {
  //    if (tc.GetDriver().JustLink()) {
  //      auto phase = BuildLinkPhase(tc, inputs, outputOpts);
  //    } else {
  //      // May want to check that you have compile
  //      auto phase = BuildLinkPhase(tc, pc, outputOpts);
  //      ac.CacheForTopLevel(phase);
  //    }
  //  }

  // if (tc.GetDriver().CanLink()) {
  //     auto linkPhase = BuildLinkPhase(tc, pc, outputOpts);
  //     ac.CacheForTopLevel(phase);
  //   }
  // }
}
void QuadraticCompilationModel::BuildJobs(ToolChain &tc, PhaseCache &pc,
                                          JobCache &jc,
                                          const OutputOptions &outputOpts) {

  if (tc.GetDriver().CanCompile()) {
    assert(pc.HasCompile());
    BuildCompileJobs(tc, pc, jc, outputOpts);
  }
}

void QuadraticCompilationModel::BuildCompileJobs(
    ToolChain &tc, PhaseCache &pc, JobCache &jc,
    const OutputOptions &outputOpts) {

  auto BuildCompileJob = [&](ToolChain &tc, const Phase &phase,
                             const OutputOptions &outputOpts) -> Job * {

  };
  for (auto input : pc.forCompile) {
    jc.CacheForCompile(BuildCompileJob(tc, *InputToPhase(input), outputOpts));
  }
}

// TODO: This may actually be generic
void QuadraticCompilationModel::BuildJobs(ToolChain &tc, const Phase &phase,
                                          JobCache &jc,
                                          const OutputOptions &outputOpts) {}

// auto BuildCompileTaskDetails = [&]() -> void {

// };

// if(tc.GetDriver().JustCompile()){

// }
// if (tc.GetDriver().GetAction().CanCompile()) {
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
//   auto phase = InputToPhase(input);
//   assert(phase);
//   auto taskDetail =
//   tc.ConstructTaskDetail(llvm::cast<CompilePhase>(*phase));
// }
// }

std::unique_ptr<Compilation> QuadraticCompilationModel::BuildCompilation(
    ToolChain &tc, const file::Files &inputs, const OutputOptions &outputOpts) {

  PhaseCache pc;
  BuildPhases(tc, inputs, pc, outputOpts);

  JobCache jc;
  BuildJobs(tc, pc, jc, outputOpts);

  // TODO: if print ....

  // TODO: it seems that we can skip these steps if we create the compilation
  // ahead of time and just do
  /// compilation.AddTaskDetail(tc.ConstructTaskDetail(llvm::cast<CompilePhase>(*phase)))

  // TODO: Check input size
  // Now, build the phase system since we have a toolchain
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
