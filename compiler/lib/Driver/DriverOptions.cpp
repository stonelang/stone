#include "stone/Driver/DriverOptions.h"
#include "stone/Diag/DiagnosticEngine.h"
#include "stone/Driver/DriverInputFile.h"

#include "llvm/Support/Host.h"
#include "llvm/Support/Path.h"

using namespace stone;

DriverOptions::DriverOptions()
    : defaultTargetTriple(llvm::sys::getDefaultTargetTriple()) {}

bool DriverOptions::IsCompilableAction() const { return action.CanCompile(); }

bool DriverOptions::IsCompileOnlyAction() const {
  return (IsCompilableAction() && !IsLinkableAction());
}
bool DriverOptions::IsLinkableAction() const {
  return (linkMode != LinkMode::None);
}

bool DriverOptions::IsLinkOnlyAction() const {
  return (IsLinkableAction() && !IsCompilableAction());
}

DriverInputsAndOutputs::DriverInputsAndOutputs() {}

DriverInputsConverter::DriverInputsConverter(const llvm::opt::ArgList &args,
                                             DriverOptions &driverOpts,
                                             DiagnosticEngine &diags)
    : args(args), driverOpts(driverOpts), diags(diags) {}

DriverOptionsConverter::DriverOptionsConverter(const llvm::opt::ArgList &args,
                                               DriverOptions &driverOpts,
                                               LangOptions &langOpts,
                                               DiagnosticEngine &diags)
    : args(args), driverOpts(driverOpts), langOpts(langOpts), diags(diags) {}

Status DriverOptionsConverter::Convert() { return Status(); }
