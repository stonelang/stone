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
class Driver;

class DiagnosticEngine;

/// Indicates whether a temporary file should always be preserved if a part of
/// the compilation crashes.
enum class PreserveOnSignal : bool { No, Yes };

class CompilationResult final {

public:
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

  Status status;

public:
  CompilationResult(const CompilationResult &) = delete;
  CompilationResult &operator=(const CompilationResult &) = delete;

  CompilationResult(CompilationResult &&) = default;
  CompilationResult &operator=(CompilationResult &&) = default;

public:
  CompilationResult();

  /// Construct a \c Compilation::CompilationResult from just an exit code.
  // static CompilationResult code(int code) {
  //   return Compilation::CompilationResult{false, code,
  //                              fine_grained_dependencies::ModuleDepGraph()};
  // }
};

class Compilation final {
  const Driver &driver;

public:
  Compilation(const Driver &driver);

public:
  Status Setup();
  const Driver &GetDriver() const { return driver; }

public:
  /// Print the list of Actions in a Compilation.
  void PrintJobs(llvm::raw_ostream &os) const;

public:
  CompilationResult ExecuteJobs();
};

} // namespace stone

#endif
