#include "stone/Driver/Driver.h"

using namespace stone;

Driver::Driver() {}

struct Driver::Module {

  Driver &driver;

  Module(Driver &driver) : driver(driver) {}

  ~Module() {}

  llvm::SmallVector<const CompileStep *> Inputs;
};

struct Driver::Linker {
  Driver &driver;

  Linker(Driver &driver) : driver(driver) {}

  ~Linker() {}

  llvm::SmallVector<const LinkStep *> Inputs;
};

void Driver::BuildTopLevelSteps() {}

Step *Driver::BuildStep() {}

void Driver::PrintSteps() {}

StepKind Driver::GetFinalStepKind(DriverActionKind kind) {
  switch (kind) {
  case DriverActionKind::Parse:
  case DriverActionKind::EmitParse:
  case DriverActionKind::ResolveImports:
  case DriverActionKind::TypeCheck:
  case DriverActionKind::EmitAST:
  case DriverActionKind::EmitIR:
  case DriverActionKind::EmitBC:
  case DriverActionKind::EmitModule:
  case DriverActionKind::MergeModules:
  case DriverActionKind::EmitObject:
  case DriverActionKind::EmitAssembly:
    return StepKind::Compile;
  case DriverActionKind::EmitLibrary:
  case DriverActionKind::EmitExecutable:
    return StepKind::Link;
  }
  assert(false && "Unknown DriverActionKind");
}
