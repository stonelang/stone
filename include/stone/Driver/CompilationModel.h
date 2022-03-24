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
  BuildCompilation(ToolChain &tc, const file::Files &inputs,
                   const OutputOptions &outputOpts) {}

  virtual void BuildIntents(ToolChain &tc, const file::Files &inputs,
                            IntentCache &ic, const OutputOptions &outputOpts) {}

  virtual void BuildJobs(ToolChain &tc, IntentCache &ic,
                         const OutputOptions &outputOpts) {}

  CompilationMode GetCompilationMode() { return mode; }

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
  Intent *ConstructCompileIntent(intent::Input input,
                                 const OutputOptions &outputOpts);

  Intent *ConstructStaticLinkIntent(intent::InputList inputs,
                                    const OutputOptions &outputOpts);

  Intent *ConstructExecLinkIntent(intent::InputList inputs,
                                  const OutputOptions &outputOpts);

  Intent *ConstructDynamicLinkIntent(intent::InputList inputs,
                                     const OutputOptions &outputOpts);
};

class QuadraticCompilationModel final : public CompilationModel {
public:
  QuadraticCompilationModel() : CompilationModel(CompilationMode::Quadratic) {}

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

// class FlatCompilationModel final : public CompilationModel {
// public:
//   FlatCompilationModel() : CompilationModel(CompilationMode::Flat) {}

// public:
//   void BuildIntents(Driver &driver, const file::Files &inputs, IntentCache
//   &ic,
//                  const OutputOptions &outputOpts) override;

//   // public:
//   //   std::unique_ptr<Compilation>
//   //   BuildCompilation(Driver &driver, const file::Files &inputs,
//   IntentCache &ic,
//   //                    const OutputOptions &outputOpts) override;
// };

// class CPUCompilationModel final : public CompilationModel {

// public:
//   CPUCompilationModel() : CompilationModel(CompilationMode::CPU) {}

// public:
//   void BuildIntents(Driver &driver, const file::Files &inputs, IntentCache
//   &ic,
//                  const OutputOptions &outputOpts) override;

//   // public:
//   //   std::unique_ptr<Compilation>
//   //   BuildCompilation(Driver &driver, const file::Files &inputs,
//   IntentCache &ic,
//   //                    const OutputOptions &outputOpts) override;
// };
// class SingleCompilationModel final : public CompilationModel {
// public:
//   SingleCompilationModel() : CompilationModel(CompilationMode::Single) {}

// public:
//   void BuildIntents(Compilation& compilation, const file::Files &inputs,
//   IntentCache &ic,
//                  const OutputOptions &outputOpts) override;

//   // public:
//   //   std::unique_ptr<Compilation>
//   //   BuildCompilation(Driver &driver, const file::Files &inputs,
//   IntentCache &ic,
//   //                    const OutputOptions &outputOpts) override;
// };

} // namespace stone

#endif
