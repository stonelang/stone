#include "stone/Driver/CompilationModel.h"
#include "stone/Driver/Driver.h"

using namespace stone;

// TODO: Look into the JobCache instead of the job::Input
/// Goal: Build the link job and CacheForTop(..)
Job *CompilationModel::BuildLinkJob(Driver &driver, ToolChain &tc, JobCache &jc,
                                    const OutputOptions &outputOpts) {

  Job *job = nullptr;
  // Make sure that we can link
  assert(driver.CanLink() && "The current mode does not allow linking.");

  // Make sure that there is stuff to link
  assert(jc.ForCompile() && "There is nothing to link.");

  switch (driver.GetLinkMode()) {
  case LinkMode::EmitExecutable:
    job = tc.ConstructExecLinkJob(jc.forCompile, outputOpts);
  case LinkMode::EmitStaticLibrary:
    job = tc.ConstructStaticLinkJob(jc.forCompile, outputOpts);
  case LinkMode::EmitDynamicLibrary:
    job = tc.ConstructDynamicLinkJob(jc.forCompile, outputOpts);
  default:
    stone::Panic("Alien link mode");
  }
  assert(job);
  jc.CacheForTop(job);

  return job;
}

Job *CompilationModel::BuildLinkJob(Driver &driver, ToolChain &tc,
                                    const file::Files &inputs,
                                    const OutputOptions &outputOpts) {
  Job *job = nullptr;

  assert(driver.JustLink() && "The current mode is only for linking");

  return nullptr;
}

void QuadraticCompilationModel::BuildCompileJobs(
    Driver &driver, ToolChain &tc, const file::Files &inputs, JobCache &jc,
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
void QuadraticCompilationModel::BuildJobs(Driver &driver, ToolChain &tc,
                                          const file::Files &inputs,
                                          JobCache &jc,
                                          const OutputOptions &outputOpts) {

  if (driver.GetMode().CanCompile()) {
    BuildCompileJobs(driver, tc, inputs, jc, outputOpts);
    if (driver.JustCompile()) {
      return;
    }
  }
  if (driver.CanLink()) {
    if (driver.JustLink()) {
      BuildLinkJob(driver, tc, inputs, outputOpts);
    } else {
      BuildLinkJob(driver, tc, jc, outputOpts);
    }
  }
}
void QuadraticCompilationModel::BuildTaskDetails(
    Driver &driver, ToolChain &tc, JobCache &jc,
    const OutputOptions &outputOpts) {

  auto BuildCompileTaskDetails = [&]() -> void {

  };

  // if(driver.JustCompile()){

  // }
  // if (driver.GetMode().CanCompile()) {
  //   BuildCompileJobs(driver, tc, inputs, jc, outputOpts);
  //   if (driver.JustCompile()) {
  //     return;
  //   }
  // }
  // if (driver.CanLink()) {
  //   if (driver.JustLink()) {
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
std::unique_ptr<Compilation>
QuadraticCompilationModel::BuildCompilation(Driver &driver, ToolChain &tc,
                                            const file::Files &inputs,
                                            const OutputOptions &outputOpts) {
  JobCache jc;
  BuildJobs(driver, tc, inputs, jc, outputOpts);
  BuildTaskDetails(driver, tc, jc, outputOpts);

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
