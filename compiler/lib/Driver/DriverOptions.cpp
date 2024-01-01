#include "stone/Driver/DriverOptions.h"
#include "stone/Diag/DiagnosticEngine.h"
#include "stone/Diag/DriverDiagnostic.h"
#include "stone/Driver/CompilationEntity.h"

#include "llvm/Support/Host.h"
#include "llvm/Support/Path.h"

using namespace stone;
using namespace llvm::opt;

///< DriverOptions
DriverOptions::DriverOptions()
    : defaultTargetTriple(llvm::sys::getDefaultTargetTriple()) {}

bool DriverOptions::IsSupportAction() const { return action.IsSupport(); }
bool DriverOptions::IsCompilableAction() const { return action.CanCompile(); }
bool DriverOptions::IsCompileOnlyAction() const {
  return (IsCompilableAction() && !IsLinkableAction());
}
bool DriverOptions::IsLinkableAction() const {
  return (driverOutputInfo.HasLinkMode());
}
bool DriverOptions::IsLinkOnlyAction() const {
  return (IsLinkableAction() && !IsCompilableAction());
}

/// < DriverInputsAndOutputs
DriverInputsAndOutputs::DriverInputsAndOutputs(
    const DriverInputsAndOutputs &other) {
  for (const DriverInputFile *input : other.inputs) {
    AddInput(input);
  }
}

DriverInputsAndOutputs &
DriverInputsAndOutputs::operator=(const DriverInputsAndOutputs &other) {
  ClearInputs();
  for (const DriverInputFile *input : other.inputs) {
    AddInput(input);
  }
  return *this;
}

void DriverInputsAndOutputs::AddInput(const DriverInputFile *input) {
  inputs.push_back(input);
}
void DriverInputsAndOutputs::ClearInputs() { inputs.clear(); }

void DriverInputsAndOutputs::ForEachInput(
    std::function<void(const DriverInputFile *)> callback) const {
  for (const DriverInputFile *input : inputs) {
    callback(input);
  }
}
