#include "stone/Driver/CompilationModel.h"

using namespace stone;

void QuadraticCompilationModel::BuildCompileJob(
    const file::File &primaryInput, const file::Files &inputs,
    const OutputOptions &outputOpts) {

  // auto job = toolChain.ConstructCompileJob(primaryInput, outputOpts);
  // for (auto &input : inputs) {
  //   /// The tool chain stores the jobs that it created.
  //   job.AddInput(input);
  // }
}

void QuadraticCompilationModel::BuildJobs(Driver &driver,
                                          const file::Files &inputs,
                                          const OutputOptions &outputOpts) {
  for (auto &input : inputs) {
    switch (input.GetType()) {
    case file::Type::Stone: {
      BuildCompileJob(input, inputs, outputOpts);
      break;
    }
    default:
      stone::Panic("Alien file -- cannot build job.");
    }
  }
}

std::unique_ptr<Compilation>
QuadraticCompilationModel::BuildCompilation(Driver &driver,
                                            const file::Files &inputs,
                                            const OutputOptions &outputOpts) {

  BuildJobs(driver, inputs, outputOpts);

  return nullptr;
}

void FlatCompilationModel::BuildJobs(Driver &driver, const file::Files &inputs,
                                     const OutputOptions &outputOpts) {}

std::unique_ptr<Compilation>
FlatCompilationModel::BuildCompilation(Driver &driver,
                                       const file::Files &inputs,
                                       const OutputOptions &outputOpts) {

  BuildJobs(driver, inputs, outputOpts);
  return nullptr;
}

void CPUCompilationModel::BuildJobs(Driver &driver, const file::Files &inputs,
                                    const OutputOptions &outputOpts) {}

std::unique_ptr<Compilation>
CPUCompilationModel::BuildCompilation(Driver &driver, const file::Files &inputs,
                                      const OutputOptions &outputOpts) {

  BuildJobs(driver, inputs, outputOpts);
  return nullptr;
}

void SingleCompilationModel::BuildJobs(Driver &driver,
                                       const file::Files &inputs,
                                       const OutputOptions &outputOpts) {}

std::unique_ptr<Compilation>
SingleCompilationModel::BuildCompilation(Driver &driver,
                                         const file::Files &inputs,
                                         const OutputOptions &outputOpts) {

  BuildJobs(driver, inputs, outputOpts);
  return nullptr;
}