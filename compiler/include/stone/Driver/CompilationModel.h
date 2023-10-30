#ifndef STONE_DRIVER_COMPILATIONMODEL_H
#define STONE_DRIVER_COMPILATIONMODEL_H

#include "stone/Basic/File.h"
#include "stone/Basic/LLVM.h"
#include "stone/Basic/List.h"
#include "stone/Drive/Compilation.h"
#include "stone/Drive/OutputOptions.h"

namespace stone {

class Driver;
class ToolChain;

class CompilationModel {

  CompilationModelKind kind;
  std::unique_ptr<Compilation> compilation;

public:
  CompilationModel(CompilationModelKind kind) : kind(kind) {}

public:
  virtual std::unique_ptr<Compilation>
  BuildCompilation(ToolChain &tc, const file::Files &inputs,
                   const OutputOptions &outputOpts) {}

  virtual void BuildJobActions(ToolChain &tc, const file::Files &inputs,
                           JobActionCache &ic, const OutputOptions &outputOpts) {}

  virtual void BuildJobs(ToolChain &tc, JobActionCache &pc, JobCache &jc,
                         const OutputOptions &outputOpts) {}

  virtual void BuildJobs(ToolChain &tc, const JobAction &phase, JobCache &jc,
                         const OutputOptions &outputOpts) {}

  CompilationModelKind GetKind() { return kind; }

protected:
  JobAction *BuildLinkJobAction(ToolChain &tc, JobActionCache &ic,
                        const OutputOptions &outputOpts);
  JobAction *BuildLinkJobAction(ToolChain &tc, const file::Files &inputs,
                        const OutputOptions &outputOpts);

protected:
  file::File *InputToFile(JobActionInput input) const {
    return input.dyn_cast<file::File *>();
  }
  JobAction *InputToJobAction(JobActionInput input) const {
    return input.dyn_cast<JobAction *>();
  }

public:
  JobAction *ConstructCompileJobAction(ToolChain &tc, JobActionInput input,
                               const OutputOptions &outputOpts);

  JobAction *ConstructStaticLinkJobAction(ToolChain &tc, JobActionInputList inputs,
                                  const OutputOptions &outputOpts);

  JobAction *ConstructExecLinkJobAction(ToolChain &tc, JobActionInputList inputs,
                                const OutputOptions &outputOpts);

  JobAction *ConstructDynamicLinkJobAction(ToolChain &tc, JobActionInputList inputs,
                                   const OutputOptions &outputOpts);
  JobAction *ConstructBackendJobAction();
  JobAction *ConstructMergeModuleJobAction();
  JobAction *ConstructModuleWrapJobAction();
};

class QuadraticCompilationModel final : public CompilationModel {
public:
  QuadraticCompilationModel()
      : CompilationModel(CompilationModelKind::Quadratic) {}

public:
  std::unique_ptr<Compilation>
  BuildCompilation(ToolChain &tc, const file::Files &inputs,
                   const OutputOptions &outputOpts) override;

  void BuildJobActions(ToolChain &tc, const file::Files &inputs, JobActionCache &ac,
                   const OutputOptions &outputOpts) override;

  void BuildJobs(ToolChain &tc, JobActionCache &ac, JobCache &jc,
                 const OutputOptions &outputOpts) override;

  void BuildJobs(ToolChain &tc, const JobAction &phase, JobCache &jc,
                 const OutputOptions &outputOpts) override;

private:
  void BuildCompileJobActions(ToolChain &tc, const file::Files &inputs,
                          JobActionCache &ic, const OutputOptions &outputOpts);

  void BuildCompileJobs(ToolChain &tc, JobActionCache &pc, JobCache &jc,
                        const OutputOptions &outputOpts);
};

// class FlatCompilationModel final : public CompilationModel {
// public:
//   FlatCompilationModel() : CompilationModel(CompilationModelKind::Flat) {}

// public:
//   std::unique_ptr<Compilation>
//   BuildCompilation(ToolChain &tc, const file::Files &inputs,
//                    const OutputOptions &outputOpts) override;

//   void BuildJobActions(ToolChain &tc, const file::Files &inputs, JobActionCache &ac,
//                    const OutputOptions &outputOpts) override;

//   void BuildJobs(ToolChain &tc, JobActionCache &ac, JobCache &jc,
//                  const OutputOptions &outputOpts) override;
// };

// class CPUCountCompilationModel final : public CompilationModel {
// public:
//   CPUCountCompilationModel()
//       : CompilationModel(CompilationModelKind::CPUCount) {}

// public:
//   std::unique_ptr<Compilation>
//   BuildCompilation(ToolChain &tc, const file::Files &inputs,
//                    const OutputOptions &outputOpts) override;

//   void BuildJobActions(ToolChain &tc, const file::Files &inputs, JobActionCache &ic,
//                    const OutputOptions &outputOpts) override;

//   void BuildJobs(ToolChain &tc, JobActionCache &ac, JobCache &jc,
//                  const OutputOptions &outputOpts) override;

// private:
//   int ComputeCPUCount() const;
// };

// class SingleCompilationModel final : public CompilationModel {
// public:
//   SingleCompilationModel() : CompilationModel(CompilationModelKind::Single)
//   {}

// public:
//   std::unique_ptr<Compilation>
//   BuildCompilation(ToolChain &tc, const file::Files &inputs,
//                    const OutputOptions &outputOpts) override;

//   void BuildJobActions(ToolChain &tc, const file::Files &inputs, JobActionCache &ac,
//                    const OutputOptions &outputOpts) override;

//   void BuildJobs(ToolChain &tc, JobActionCache &ic, JobCache &jc,
//                  const OutputOptions &outputOpts) override;
// };

} // namespace stone

#endif
