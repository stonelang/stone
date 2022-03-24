#include "stone/Driver/CompilationModel.h"
#include "stone/Driver/Driver.h"

using namespace stone;

Job *CompilationModel::ConstructCompileJob(job::Input input,
                                           const OutputOptions &outputOpts) {
  return nullptr;
}
Job *CompilationModel::ConstructStaticLinkJob(job::InputList inputs,
                                              const OutputOptions &outputOpts) {
  return nullptr;
}

Job *CompilationModel::ConstructExecLinkJob(job::InputList inputs,
                                            const OutputOptions &outputOpts) {
  return nullptr;
}

Job *CompilationModel::ConstructDynamicLinkJob(
    job::InputList inputs, const OutputOptions &outputOpts) {
  return nullptr;
}

// TODO: Look into the JobCache instead of the job::Input
/// Goal: Build the link job and CacheForTop(..)
Job *CompilationModel::BuildLinkJob(ToolChain &tc, JobCache &jc,
                                    const OutputOptions &outputOpts) {

  Job *job = nullptr;
  // Make sure that we can link
  assert(tc.GetDriver().CanLink() &&
         "The current mode does not allow linking.");

  // Make sure that there is stuff to link
  assert(jc.ForCompile() && "There is nothing to link");

  switch (tc.GetDriver().GetLinkMode()) {
  case LinkMode::EmitExecutable:
    job = tc.ConstructExecLinkJob(jc.forCompile, outputOpts);
    break;
  case LinkMode::EmitStaticLibrary:
    job = tc.ConstructStaticLinkJob(jc.forCompile, outputOpts);
    break;
  case LinkMode::EmitDynamicLibrary:
    job = tc.ConstructDynamicLinkJob(jc.forCompile, outputOpts);
    break;
  default:
    stone::Panic("Alien link mode");
  }
  assert(job);
  jc.CacheForTop(job);
  return job;
}

Job *CompilationModel::BuildLinkJob(ToolChain &tc, const file::Files &inputs,
                                    const OutputOptions &outputOpts) {
  Job *job = nullptr;

  assert(tc.GetDriver().JustLink() && "The current mode is only for linking");

  return nullptr;
}

void QuadraticCompilationModel::BuildCompileJobs(
    ToolChain &tc, const file::Files &inputs, JobCache &jc,
    const OutputOptions &outputOpts) {

  auto BuildCompileJob = [&](const file::File &primaryInput,
                             const file::Files &inputs,
                             const OutputOptions &outputOpts) -> Job * {
    auto job = tc.ConstructCompileJob(primaryInput, outputOpts);
    assert(job);
    for (auto &input : inputs) {
      /// The tool chain stores the jobs that it created.
      job->AddInput(const_cast<file::File *>(&input));
    }
    return job;
  };
  for (auto &input : inputs) {
    assert(input.GetType() == file::Type::Stone); // Only file-type for now
    auto job = BuildCompileJob(input, inputs, outputOpts);
    jc.CacheForCompile(job);
  }
}
// Goal :
void QuadraticCompilationModel::BuildJobs(ToolChain &tc,
                                          const file::Files &inputs,
                                          JobCache &jc,
                                          const OutputOptions &outputOpts) {
  if (tc.GetDriver().GetMode().CanCompile()) {
    BuildCompileJobs(tc, inputs, jc, outputOpts);
    if (tc.GetDriver().JustCompile()) {
      return;
    }
  }
  if (tc.GetDriver().CanLink()) {
    if (tc.GetDriver().JustLink()) {
      BuildLinkJob(tc, inputs, outputOpts);
    } else {
      BuildLinkJob(tc, jc, outputOpts);
    }
  }
}
void QuadraticCompilationModel::BuildTaskDetails(
    ToolChain &tc, JobCache &jc, const OutputOptions &outputOpts) {

  auto BuildCompileTaskDetails = [&]() -> void {

  };

  // if(tc.GetDriver().JustCompile()){

  // }
  // if (tc.GetDriver().GetMode().CanCompile()) {
  //   BuildCompileJobs(driver, tc, inputs, jc, outputOpts);
  //   if (tc.GetDriver().JustCompile()) {
  //     return;
  //   }
  // }
  // if (tc.GetDriver().CanLink()) {
  //   if (tc.GetDriver().JustLink()) {
  //     BuildLinkJob(driver, tc, inputs, outputOpts);
  //   } else {
  //     BuildLinkJob(driver, tc, jc, outputOpts);
  //   }
  // }

  // // if we have nothing to do, we return
  // if (jc.ForCompile()) {
  //   return nullptr;
  // }
  // for (auto input : jc.forCompile) {
  //   auto job = InputToJob(input);
  //   assert(job);
  //   auto taskDetail = tc.ConstructTaskDetail(llvm::cast<CompileJob>(*job));
  // }
}
std::unique_ptr<Compilation> QuadraticCompilationModel::BuildCompilation(
    ToolChain &tc, const file::Files &inputs, const OutputOptions &outputOpts) {
  JobCache jc;
  BuildJobs(tc, inputs, jc, outputOpts);
  BuildTaskDetails(tc, jc, outputOpts);

  // TODO: if print ....

  // TODO: it seems that we can skip these steps if we create the compilation
  // ahead of time and just do
  /// compilation.AddTaskDetail(tc.ConstructTaskDetail(llvm::cast<CompileJob>(*job)))

  // TODO: Check input size
  // Now, build the job system since we have a toolchain
  // auto compilation =
  //     std::make_unique<Compilation>(*this, toolChain, std::move(dal));

  return nullptr;
}

// void FlatCompilationModel::BuildJobs(Compilation& compilation, const
// file::Files &inputs,
//                                      JobCache &jc,
//                                      const OutputOptions &outputOpts) {}

// std::unique_ptr<Compilation>
// FlatCompilationModel::BuildCompilation(Driver &driver,
//                                        const file::Files &inputs, JobCache
//                                        &jc, const OutputOptions &outputOpts)
//                                        {

//   BuildJobs(driver, inputs, jc, outputOpts);
//   return nullptr;
// }

// void CPUCompilationModel::BuildJobs(Compilation& compilation, const
// file::Files &inputs,
//                                     JobCache &jc,
//                                     const OutputOptions &outputOpts) {}

// std::unique_ptr<Compilation>
// CPUCompilationModel::BuildCompilation(Driver &driver, const file::Files
// &inputs,
//                                       JobCache &jc,
//                                       const OutputOptions &outputOpts) {

//   BuildJobs(driver, inputs, jc, outputOpts);
//   return nullptr;
// }

// void SingleCompilationModel::BuildJobs(Compilation& compilation,
//                                        const file::Files &inputs, JobCache
//                                        &jc, const OutputOptions &outputOpts)
//                                        {}

// std::unique_ptr<Compilation> SingleCompilationModel::BuildCompilation(
//     Driver &driver, const file::Files &inputs, JobCache &jc,
//     const OutputOptions &outputOpts) {

//   BuildJobs(driver, inputs, jc, outputOpts);
//   return nullptr;
// }
