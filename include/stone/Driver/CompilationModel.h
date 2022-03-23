#ifndef STONE_DRIVER_COMPILATIONMODEL_H
#define STONE_DRIVER_COMPILATIONMODEL_H

#include "stone/Core/File.h"
#include "stone/Core/LLVM.h"
#include "stone/Core/List.h"
#include "stone/Driver/Compilation.h"
#include "stone/Driver/OutputOptions.h"

namespace stone {

class Driver;
class CompilationModel {

  CompilationMode mode;
  std::unique_ptr<Compilation> compilation;

public:
  CompilationModel(CompilationMode mode) : mode(mode) {}

public:
  virtual std::unique_ptr<Compilation>
  BuildCompilation(Driver &driver, ToolChain &tc, const file::Files &inputs,
                   const OutputOptions &outputOpts) {}

  virtual void BuildJobs(Driver &driver, ToolChain &tc,
                         const file::Files &inputs, JobCache &jc,
                         const OutputOptions &outputOpts) {}

  virtual void BuildTaskDetails(Driver &driver, ToolChain &tc, JobCache &jc,
                         const OutputOptions &outputOpts) {}


  CompilationMode GetCompilationMode() { return mode; }

protected:
  Job *BuildLinkJob(Driver &driver, ToolChain &tc, JobCache &jc,
                    const OutputOptions &outputOpts);
  Job *BuildLinkJob(Driver &driver, ToolChain &tc, const file::Files &inputs,
                    const OutputOptions &outputOpts);

protected:
  file::File *InputToFile(job::Input input) const {
    return input.dyn_cast<file::File *>();
  }
  Job *InputToJob(job::Input input) const { return input.dyn_cast<Job *>(); }
};

class QuadraticCompilationModel final : public CompilationModel {
public:
  QuadraticCompilationModel() : CompilationModel(CompilationMode::Quadratic) {}

public:
  std::unique_ptr<Compilation>
  BuildCompilation(Driver &driver, ToolChain &tc, const file::Files &inputs,
                   const OutputOptions &outputOpts) override;

  void BuildJobs(Driver &driver, ToolChain &tc, const file::Files &inputs,
                 JobCache &jc, const OutputOptions &outputOpts) override;

private:
  void BuildCompileJobs(Driver &driver, ToolChain &tc,
                        const file::Files &inputs, JobCache &jc,
                        const OutputOptions &outputOpts);
};

// class FlatCompilationModel final : public CompilationModel {
// public:
//   FlatCompilationModel() : CompilationModel(CompilationMode::Flat) {}

// public:
//   void BuildJobs(Driver &driver, const file::Files &inputs, JobCache &jc,
//                  const OutputOptions &outputOpts) override;

//   // public:
//   //   std::unique_ptr<Compilation>
//   //   BuildCompilation(Driver &driver, const file::Files &inputs, JobCache
//   &jc,
//   //                    const OutputOptions &outputOpts) override;
// };

// class CPUCompilationModel final : public CompilationModel {

// public:
//   CPUCompilationModel() : CompilationModel(CompilationMode::CPU) {}

// public:
//   void BuildJobs(Driver &driver, const file::Files &inputs, JobCache &jc,
//                  const OutputOptions &outputOpts) override;

//   // public:
//   //   std::unique_ptr<Compilation>
//   //   BuildCompilation(Driver &driver, const file::Files &inputs, JobCache
//   &jc,
//   //                    const OutputOptions &outputOpts) override;
// };
// class SingleCompilationModel final : public CompilationModel {
// public:
//   SingleCompilationModel() : CompilationModel(CompilationMode::Single) {}

// public:
//   void BuildJobs(Compilation& compilation, const file::Files &inputs,
//   JobCache &jc,
//                  const OutputOptions &outputOpts) override;

//   // public:
//   //   std::unique_ptr<Compilation>
//   //   BuildCompilation(Driver &driver, const file::Files &inputs, JobCache
//   &jc,
//   //                    const OutputOptions &outputOpts) override;
// };

} // namespace stone

#endif
