#include "stone/Driver/Driver.h"
#include "stone/Driver/Step.h"

using namespace stone;

llvm::ArrayRef<StepKind> DriverOptions::GetStepKindList(DriverActionKind kind) {

  switch (kind) {
  case DriverActionKind::Parse:
  case DriverActionKind::EmitParse:
  case DriverActionKind::ResolveImports:
  case DriverActionKind::TypeCheck:
  case DriverActionKind::EmitAST:
  case DriverActionKind::EmitIR:
  case DriverActionKind::EmitBC:
  case DriverActionKind::EmitModule:
  case DriverActionKind::EmitObject:
  case DriverActionKind::EmitAssembly: {
    return {StepKind::Compile};
  }
  case DriverActionKind::EmitExecutable:
  case DriverActionKind::EmitLibrary: {
    return {StepKind::Compile, StepKind::Link};
  }
  }
}
