#ifndef STONE_DRIVER_COMPILATIONMODEL_H
#define STONE_DRIVER_COMPILATIONMODEL_H

#include "stone/Basic/File.h"
#include "stone/Basic/LLVM.h"
#include "stone/Basic/List.h"
#include "stone/Driver/Compilation.h"
#include "stone/Driver/OutputOptions.h"

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

  virtual void BuildPhases(ToolChain &tc, const file::Files &inputs,
                           PhaseCache &ic, const OutputOptions &outputOpts) {}

  virtual void BuildJobs(ToolChain &tc, PhaseCache &pc, JobCache &jc,
                         const OutputOptions &outputOpts) {}

  virtual void BuildJobs(ToolChain &tc, const Phase &phase, JobCache &jc,
                         const OutputOptions &outputOpts) {}

  CompilationModelKind GetKind() { return kind; }

protected:
  Phase *BuildLinkPhase(ToolChain &tc, PhaseCache &ic,
                        const OutputOptions &outputOpts);
  Phase *BuildLinkPhase(ToolChain &tc, const file::Files &inputs,
                        const OutputOptions &outputOpts);

protected:
  file::File *InputToFile(PhaseInput input) const {
    return input.dyn_cast<file::File *>();
  }
  Phase *InputToPhase(PhaseInput input) const {
    return input.dyn_cast<Phase *>();
  }

public:
  Phase *ConstructCompilePhase(ToolChain &tc, PhaseInput input,
                               const OutputOptions &outputOpts);

  Phase *ConstructStaticLinkPhase(ToolChain &tc, PhaseInputList inputs,
                                  const OutputOptions &outputOpts);

  Phase *ConstructExecLinkPhase(ToolChain &tc, PhaseInputList inputs,
                                const OutputOptions &outputOpts);

  Phase *ConstructDynamicLinkPhase(ToolChain &tc, PhaseInputList inputs,
                                   const OutputOptions &outputOpts);
};

class QuadraticCompilationModel final : public CompilationModel {
public:
  QuadraticCompilationModel()
      : CompilationModel(CompilationModelKind::Quadratic) {}

public:
  std::unique_ptr<Compilation>
  BuildCompilation(ToolChain &tc, const file::Files &inputs,
                   const OutputOptions &outputOpts) override;

  void BuildPhases(ToolChain &tc, const file::Files &inputs, PhaseCache &ac,
                   const OutputOptions &outputOpts) override;

  void BuildJobs(ToolChain &tc, PhaseCache &ac, JobCache &jc,
                 const OutputOptions &outputOpts) override;

  void BuildJobs(ToolChain &tc, const Phase &phase, JobCache &jc,
                 const OutputOptions &outputOpts) override;

private:
  void BuildCompilePhases(ToolChain &tc, const file::Files &inputs,
                          PhaseCache &ic, const OutputOptions &outputOpts);
};

// class FlatCompilationModel final : public CompilationModel {
// public:
//   FlatCompilationModel() : CompilationModel(CompilationModelKind::Flat) {}

// public:
//   std::unique_ptr<Compilation>
//   BuildCompilation(ToolChain &tc, const file::Files &inputs,
//                    const OutputOptions &outputOpts) override;

//   void BuildPhases(ToolChain &tc, const file::Files &inputs, PhaseCache &ac,
//                    const OutputOptions &outputOpts) override;

//   void BuildJobs(ToolChain &tc, PhaseCache &ac, JobCache &jc,
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

//   void BuildPhases(ToolChain &tc, const file::Files &inputs, PhaseCache &ic,
//                    const OutputOptions &outputOpts) override;

//   void BuildJobs(ToolChain &tc, PhaseCache &ac, JobCache &jc,
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

//   void BuildPhases(ToolChain &tc, const file::Files &inputs, PhaseCache &ac,
//                    const OutputOptions &outputOpts) override;

//   void BuildJobs(ToolChain &tc, PhaseCache &ic, JobCache &jc,
//                  const OutputOptions &outputOpts) override;
// };

} // namespace stone

#endif
