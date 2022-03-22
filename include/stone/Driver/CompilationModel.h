#ifndef STONE_DRIVER_COMPILATIONMODEL_H
#define STONE_DRIVER_COMPILATIONMODEl_H

#include "stone/Core/LLVM.h"
#include "stone/Core/List.h"
#include "stone/Driver/Compilation.h"
#include "stone/Driver/OutputOptions.h"
namespace stone {

class CompilationModel {
  CompilationMode mode;

public:
  CompilationModel(CompilationMode mode) : mode(mode) {}

protected:
  virtual void BuildJobs(/* file::Files& inputs*/) {}

public:
  virtual std::unique_ptr<Compilation>
  BuildCompilation(/*file::Files& inputs*/) {}
};

/// n inputs, n compile(s), n * n  parses(s):
/// Ex: compile_1(1=p ,...,n),..., compile_n+1(1,...., n+1=p, n),
/// compile_n(1,....,n=p)
class QuadraticCompilationModel final : public CompilationModel {
public:
  QuadraticCompilationModel() : CompilationModel(CompilationMode::Quadratic) {}

protected:
  void BuildJobs() override {}

public:
  std::unique_ptr<Compilation> BuildCompilation() override {}
};

// n input(s), n compile(s), n parse(s). Ex: compile_1(1),...., compile_n(n)
class FlatCompilationModel final : public CompilationModel {
public:
  FlatCompilationModel() : CompilationModel(CompilationMode::Flat) {}

protected:
  void BuildJobs() override {}

public:
  std::unique_ptr<Compilation> BuildCompilation() override {}
};

/// n inputs, j CPU(s), j compile(s), n * j parses
/// Ex: compile_1(1=p,...,n),..., compile_2(1,2=p..,n ), compile_j(1,...,p=j,n)
class CPUCompilationModel final : public CompilationModel {

public:
  CPUCompilationModel() : CompilationModel(CompilationMode::CPU) {}

protected:
  void BuildJobs() override {}

public:
  std::unique_ptr<Compilation> BuildCompilation() override {}
};
/// n inputs
/// Ex: compile_1(1,....,n)
class SingleCompilationModel final : public CompilationModel {
public:
  SingleCompilationModel() : CompilationModel(CompilationMode::Single) {}

protected:
  void BuildJobs() override {}

public:
  std::unique_ptr<Compilation> BuildCompilation() override {}
};

} // namespace stone

#endif
