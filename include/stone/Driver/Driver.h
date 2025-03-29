#ifndef STONE_DRIVER_DRIVER_H
#define STONE_DRIVER_DRIVER_H

#include "stone/AST/Diagnostics.h"
#include "stone/Driver/DriverOptions.h"
#include "stone/Driver/Step.h"

#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Allocator.h"

#include <functional>
#include <memory>
#include <string>

namespace llvm {
namespace opt {
class Arg;
class ArgList;
class OptTable;
class InputArgList;
class DerivedArgList;
} // namespace opt
} // namespace llvm

namespace stone {

class Driver final {

  struct Module;
  struct Linker;

  SrcMgr srcMgr;
  DiagnosticEngine diags{srcMgr};
  DriverOptions driverOpts;

  /// The allocator used to create Driver objects.
  /// These objects will be released after the Driver is destroyed.
  mutable llvm::BumpPtrAllocator allocator;

public:
  Driver();

public:
  void AddDiagnosticConsumer(DiagnosticConsumer &consumer) {
    diags.addConsumer(consumer);
  }
  void RemoveDiagnosticConsumer(DiagnosticConsumer &consumer) {
    diags.removeConsumer(consumer);
  }
  /// Set the main exec path
  void SetMainExecutablePath(llvm::StringRef executablePath) {
    driverOpts.SetMainExecutablePath(executablePath);
  }
  /// Set the main exec path
  void SetMainExecutableName(llvm::StringRef executableName) {
    driverOpts.SetMainExecutableName(executableName);
  }

public:
  DiagnosticEngine &GetDiags() { return diags; }
  const DiagnosticEngine &GetDiags() const { return diags; }

  bool HasError() const { return diags.hadAnyError(); }
  SrcMgr &GetSrcMgr() { return srcMgr; }

  DriverOptions &GetDriverOptions() { return driverOpts; }

public:
  void BuildTopLevelSteps();
  Step *BuildStep();
  void PrintSteps();
  StepKind GetFinalStepKind(DriverActionKind kind);

public:
  llvm::BumpPtrAllocator &GetAllocator() { return allocator; }
  /// Allocate - Allocate memory from the Driver bump pointer.
  void *Allocate(unsigned long bytes, unsigned alignment = 8) const {
    if (bytes == 0) {
      return nullptr;
    }
    return allocator.Allocate(bytes, alignment);
  }
};

} // namespace stone
#endif