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

public:
  CompilationModel(CompilationMode mode) : mode(mode) {}

protected:
  virtual void BuildJobs(Driver &driver, const file::Files &inputs,
                         const OutputOptions &outputOpts) {}

public:
  virtual std::unique_ptr<Compilation>
  BuildCompilation(Driver &driver, const file::Files &inputs,
                   const OutputOptions &outputOpts) {}
};

class QuadraticCompilationModel final : public CompilationModel {
public:
  QuadraticCompilationModel() : CompilationModel(CompilationMode::Quadratic) {}

protected:
  void BuildJobs(Driver &driver, const file::Files &inputs,
                 const OutputOptions &outputOpts) override;

public:
  void BuildCompileJob(const file::File &input, const file::Files &inputs,
                       const OutputOptions &outputOpts);

  std::unique_ptr<Compilation>
  BuildCompilation(Driver &driver, const file::Files &inputs,
                   const OutputOptions &outputOpts) override;
};

class FlatCompilationModel final : public CompilationModel {
public:
  FlatCompilationModel() : CompilationModel(CompilationMode::Flat) {}

protected:
  void BuildJobs(Driver &driver, const file::Files &inputs,
                 const OutputOptions &outputOpts) override;

public:
  std::unique_ptr<Compilation>
  BuildCompilation(Driver &driver, const file::Files &inputs,
                   const OutputOptions &outputOpts) override;
};

class CPUCompilationModel final : public CompilationModel {

public:
  CPUCompilationModel() : CompilationModel(CompilationMode::CPU) {}

protected:
  void BuildJobs(Driver &driver, const file::Files &inputs,
                 const OutputOptions &outputOpts) override;

public:
  std::unique_ptr<Compilation>
  BuildCompilation(Driver &driver, const file::Files &inputs,
                   const OutputOptions &outputOpts) override;
};
class SingleCompilationModel final : public CompilationModel {
public:
  SingleCompilationModel() : CompilationModel(CompilationMode::Single) {}

protected:
  void BuildJobs(Driver &driver, const file::Files &inputs,
                 const OutputOptions &outputOpts) override;

public:
  std::unique_ptr<Compilation>
  BuildCompilation(Driver &driver, const file::Files &inputs,
                   const OutputOptions &outputOpts) override;
};

} // namespace stone

#endif
