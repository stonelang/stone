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

  virtual void BuildIntents(ToolChain &tc, const file::Files &inputs,
                            IntentCache &ic, const OutputOptions &outputOpts) {}

  virtual void BuildJobs(ToolChain &tc, IntentCache &ic,
                         const OutputOptions &outputOpts) {}

  CompilationModelKind GetKind() { return kind; }

protected:
  Intent *BuildLinkIntent(ToolChain &tc, IntentCache &ic,
                          const OutputOptions &outputOpts);
  Intent *BuildLinkIntent(ToolChain &tc, const file::Files &inputs,
                          const OutputOptions &outputOpts);

protected:
  file::File *InputToFile(intent::Input input) const {
    return input.dyn_cast<file::File *>();
  }
  Intent *InputToIntent(intent::Input input) const {
    return input.dyn_cast<Intent *>();
  }

public:
  Intent *ConstructCompileIntent(ToolChain &tc, intent::Input input,
                                 const OutputOptions &outputOpts);

  Intent *ConstructStaticLinkIntent(ToolChain &tc, intent::InputList inputs,
                                    const OutputOptions &outputOpts);

  Intent *ConstructExecLinkIntent(ToolChain &tc, intent::InputList inputs,
                                  const OutputOptions &outputOpts);

  Intent *ConstructDynamicLinkIntent(ToolChain &tc, intent::InputList inputs,
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

  void BuildIntents(ToolChain &tc, const file::Files &inputs, IntentCache &ic,
                    const OutputOptions &outputOpts) override;

  void BuildJobs(ToolChain &tc, IntentCache &ic,
                 const OutputOptions &outputOpts) override;

private:
  void BuildCompileIntents(ToolChain &tc, const file::Files &inputs,
                           IntentCache &ic, const OutputOptions &outputOpts);
};

class FlatCompilationModel final : public CompilationModel {
public:
  FlatCompilationModel() : CompilationModel(CompilationModelKind::Flat) {}

public:
  std::unique_ptr<Compilation>
  BuildCompilation(ToolChain &tc, const file::Files &inputs,
                   const OutputOptions &outputOpts) override;

  void BuildIntents(ToolChain &tc, const file::Files &inputs, IntentCache &ic,
                    const OutputOptions &outputOpts) override;

  void BuildJobs(ToolChain &tc, IntentCache &ic,
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

  void BuildIntents(ToolChain &tc, const file::Files &inputs, IntentCache &ic,
                    const OutputOptions &outputOpts) override;

  void BuildJobs(ToolChain &tc, IntentCache &ic,
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

  void BuildIntents(ToolChain &tc, const file::Files &inputs, IntentCache &ic,
                    const OutputOptions &outputOpts) override;

  void BuildJobs(ToolChain &tc, IntentCache &ic,
                 const OutputOptions &outputOpts) override;
};

} // namespace stone

#endif
