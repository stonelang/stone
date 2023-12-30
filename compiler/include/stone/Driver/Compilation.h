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

/// An enum providing different levels of output which should be produced
/// by a Compilation.
enum class CompilationOutputLevel {
  /// Indicates that normal output should be produced.
  Normal,

  /// Indicates that only jobs should be printed and not run. (-###)
  PrintJobs,

  /// Indicates that verbose output should be produced. (-v)
  Verbose,

  /// Indicates that parseable output should be produced.
  Parseable,
};

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

  /// When the build was started.
  ///
  /// This should be as close as possible to when the driver was invoked, since
  /// it's used as a lower bound.
  llvm::sys::TimePoint<> compilationStartTime;

  /// The time of the last compilation.
  ///
  /// If unknown, this will be some time in the past.
  llvm::sys::TimePoint<> compilationLastTime = llvm::sys::TimePoint<>::min();

public:
  Compilation(const Driver &driver);

public:
  const Driver &GetDriver() const { return driver; }
};

} // namespace stone

#endif
