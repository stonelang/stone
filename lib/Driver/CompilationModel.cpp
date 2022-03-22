#include "stone/Driver/CompilationModel.h"
#include "stone/Driver/Driver.h"

using namespace stone;

// TODO: Look into the JobCache instead of the job::Input
Job *CompilationModel::BuildLinkJob(Driver &driver, JobCache &jc,
                                    const OutputOptions &outputOpts) {
  // Just an assert for now
  assert(jc.forCompile.size() > 0);

  return nullptr;
}

Job *CompilationModel::BuildLinkJob(Driver &driver, const file::Files &inputs,
                                    const OutputOptions &outputOpts) {

  assert(inputs.size() > 0);

  return nullptr;
}

void QuadraticCompilationModel::BuildCompileJobs(
    Driver &driver, const file::Files &inputs, JobCache &jc,
    const OutputOptions &outputOpts) {

  auto BuildCompileJob = [&](const file::File &primaryInput,
                             const file::Files &inputs,
                             const OutputOptions &outputOpts) -> Job * {
    // auto job = toolChain.ConstructCompileJob(primaryInput, outputOpts);
    // for (auto &input : inputs) {
    //   /// The tool chain stores the jobs that it created.
    //   job.AddInput(input);
    // }
    return nullptr;
  };
  for (auto &input : inputs) {
    assert(input.GetType() == file::Type::Stone); // Only file-type for now
    auto job = BuildCompileJob(input, inputs, outputOpts);
    // We always add there in the event we are printing
    jc.forCompile.push_back(job);

    // Add to cache
  }
}
void QuadraticCompilationModel::BuildJobs(Driver &driver,
                                          const file::Files &inputs,
                                          JobCache &jc,
                                          const OutputOptions &outputOpts) {
  if (driver.GetMode().CanCompile()) {
    BuildCompileJobs(driver, inputs, jc, outputOpts);
    if (driver.JustCompile()) {
      return;
    }
  }
  if (driver.CanLink()) {
    if (driver.JustLink()) {
      BuildLinkJob(driver, inputs, outputOpts);
    } else {
      BuildLinkJob(driver, jc, outputOpts);
    }
  }
}

std::unique_ptr<Compilation> QuadraticCompilationModel::BuildCompilation(
    Driver &driver, const file::Files &inputs, JobCache &jc,
    const OutputOptions &outputOpts) {

  BuildJobs(driver, inputs, jc, outputOpts);

  return nullptr;
}

void FlatCompilationModel::BuildJobs(Driver &driver, const file::Files &inputs,
                                     JobCache &jc,
                                     const OutputOptions &outputOpts) {}

std::unique_ptr<Compilation>
FlatCompilationModel::BuildCompilation(Driver &driver,
                                       const file::Files &inputs, JobCache &jc,
                                       const OutputOptions &outputOpts) {

  BuildJobs(driver, inputs, jc, outputOpts);
  return nullptr;
}

void CPUCompilationModel::BuildJobs(Driver &driver, const file::Files &inputs,
                                    JobCache &jc,
                                    const OutputOptions &outputOpts) {}

std::unique_ptr<Compilation>
CPUCompilationModel::BuildCompilation(Driver &driver, const file::Files &inputs,
                                      JobCache &jc,
                                      const OutputOptions &outputOpts) {

  BuildJobs(driver, inputs, jc, outputOpts);
  return nullptr;
}

void SingleCompilationModel::BuildJobs(Driver &driver,
                                       const file::Files &inputs, JobCache &jc,
                                       const OutputOptions &outputOpts) {}

std::unique_ptr<Compilation> SingleCompilationModel::BuildCompilation(
    Driver &driver, const file::Files &inputs, JobCache &jc,
    const OutputOptions &outputOpts) {

  BuildJobs(driver, inputs, jc, outputOpts);
  return nullptr;
}