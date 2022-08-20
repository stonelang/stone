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

  virtual void BuildActions(ToolChain &tc, const file::Files &inputs,
                            ActionCache &ic, const OutputOptions &outputOpts) {}

  virtual void BuildJobs(ToolChain &tc, ActionCache &ic, JobCache &jc,
                         const OutputOptions &outputOpts) {}

  CompilationModelKind GetKind() { return kind; }

protected:
  Action *BuildLinkAction(ToolChain &tc, ActionCache &ic,
                          const OutputOptions &outputOpts);
  Action *BuildLinkAction(ToolChain &tc, const file::Files &inputs,
                          const OutputOptions &outputOpts);

protected:
  file::File *InputToFile(action::Input input) const {
    return input.dyn_cast<file::File *>();
  }
  Action *InputToAction(action::Input input) const {
    return input.dyn_cast<Action *>();
  }

public:
  Action *ConstructCompileAction(ToolChain &tc, action::Input input,
                                 const OutputOptions &outputOpts);

  Action *ConstructStaticLinkAction(ToolChain &tc, action::InputList inputs,
                                    const OutputOptions &outputOpts);

  Action *ConstructExecLinkAction(ToolChain &tc, action::InputList inputs,
                                  const OutputOptions &outputOpts);

  Action *ConstructDynamicLinkAction(ToolChain &tc, action::InputList inputs,
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

  void BuildActions(ToolChain &tc, const file::Files &inputs, ActionCache &ac,
                    const OutputOptions &outputOpts) override;

  void BuildJobs(ToolChain &tc, ActionCache &ac, JobCache &jc,
                 const OutputOptions &outputOpts) override;

private:
  void BuildCompileActions(ToolChain &tc, const file::Files &inputs,
                           ActionCache &ic, const OutputOptions &outputOpts);
};

class FlatCompilationModel final : public CompilationModel {
public:
  FlatCompilationModel() : CompilationModel(CompilationModelKind::Flat) {}

public:
  std::unique_ptr<Compilation>
  BuildCompilation(ToolChain &tc, const file::Files &inputs,
                   const OutputOptions &outputOpts) override;

  void BuildActions(ToolChain &tc, const file::Files &inputs, ActionCache &ac,
                    const OutputOptions &outputOpts) override;

  void BuildJobs(ToolChain &tc, ActionCache &ac, JobCache &jc,
                 const OutputOptions &outputOpts) override;
};

class CPUCountCompilationModel final : public CompilationModel {
public:
  CPUCountCompilationModel()
      : CompilationModel(CompilationModelKind::CPUCount) {}

public:
  std::unique_ptr<Compilation>
  BuildCompilation(ToolChain &tc, const file::Files &inputs,
                   const OutputOptions &outputOpts) override;

  void BuildActions(ToolChain &tc, const file::Files &inputs, ActionCache &ic,
                    const OutputOptions &outputOpts) override;

  void BuildJobs(ToolChain &tc, ActionCache &ac, JobCache &jc,
                 const OutputOptions &outputOpts) override;

private:
  int ComputeCPUCount() const;
};

class SingleCompilationModel final : public CompilationModel {
public:
  SingleCompilationModel() : CompilationModel(CompilationModelKind::Single) {}

public:
  std::unique_ptr<Compilation>
  BuildCompilation(ToolChain &tc, const file::Files &inputs,
                   const OutputOptions &outputOpts) override;

  void BuildActions(ToolChain &tc, const file::Files &inputs, ActionCache &ac,
                    const OutputOptions &outputOpts) override;

  void BuildJobs(ToolChain &tc, ActionCache &ic, JobCache &jc,
                 const OutputOptions &outputOpts) override;
};

} // namespace stone

#endif
