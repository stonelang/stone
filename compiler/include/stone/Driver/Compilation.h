#ifndef STONE_DRIVER_DRIVER_COMPILATION_H
#define STONE_DRIVER_DRIVER_COMPILATION_H

#include "stone/Driver/Driver.h"
#include "stone/Driver/Job.h"

#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Chrono.h"

#include <memory>
#include <vector>

namespace llvm {
namespace opt {
class InputArgList;
class DerivedArgList;
} // namespace opt
} // namespace llvm

namespace stone {
class Job;
class JobConstruction;
class DiagnosticEngine;

/// Indicates whether a temporary file should always be preserved if a part of
/// the compilation crashes.
enum class PreserveOnSignal : bool { No, Yes };

class CompilationResult final {
  /// Set to true if any job exits abnormally (i.e. crashes).
  bool hadAbnormalExit;
  /// The exit code of this driver process.
  int exitCode;
  /// The dependency graph built up during the compilation of this module.
  ///
  /// This data is used for cross-module module dependencies.
  // fine_grained_dependencies::ModuleDepGraph depGraph;

  // CompilationResult(bool hadAbnormalExit, int exitCode,
  //        fine_grained_dependencies::ModuleDepGraph depGraph)
  //     : hadAbnormalExit(hadAbnormalExit), exitCode(exitCode),
  //       depGraph(std::move(depGraph)) {}

public:
  CompilationResult(const CompilationResult &) = delete;
  CompilationResult &operator=(const CompilationResult &) = delete;

  CompilationResult(CompilationResult &&) = default;
  CompilationResult &operator=(CompilationResult &&) = default;

  /// Construct a \c Compilation::CompilationResult from just an exit code.
  // static CompilationResult code(int code) {
  //   return Compilation::CompilationResult{false, code,
  //                              fine_grained_dependencies::ModuleDepGraph()};
  // }
};

class Compilation final {
  const Driver &driver;

  /// The Jobs which will be performed by this compilation.
  llvm::SmallVector<const Job *, 32> jobs;

public:
  Compilation(const Driver &driver);

public:
  const Driver &GetDriver() const { return driver; }
};

} // namespace stone

#endif
