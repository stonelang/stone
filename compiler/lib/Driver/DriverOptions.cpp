#include "stone/Driver/DriverOptions.h"

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