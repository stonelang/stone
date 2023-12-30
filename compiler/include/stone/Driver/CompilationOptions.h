#ifndef STONE_DRIVER_COMPILATION_OPTIONS_H
#define STONE_DRIVER_COMPILATION_OPTIONS_H

#include "stone/Basic/File.h"
#include "stone/Basic/STDAlias.h"

namespace stone {

enum class LinkMode : UInt8 {
  // We are not linking
  None = 0,
  // The default output compiling -- sc looks afor a main file and
  // outputs an executable file
  Executable,

  // The default library output: 'stone test.stone -emit-library -> test.dylib'
  DynamicLibrary,

  // The Library output that requires static: 'stone test.stone -emit-library
  // -satic -> test.a'
  StaticLibrary
};

/// This mode controls the compilation process
/// p := -primary-file
enum class CompilationKind : UInt8 {
  /// n input (s), n compile(s), n * n  parses
  /// Ex: compile_1(1=p ,...,n), compile_2(1,2=p,...,n),...,
  /// compile_n(1,....,n=p)
  Normal = 0,
  /// n inputs, 1 compile, n parses, p := 0
  /// Ex: compile(1,....,n)
  Single,
  /// n input(s), n compile(s), n parses
  /// Ex: compile_1(1=p), compile_2(2=p),..., compile_n(n=p)
  Flat,
  /// n input (s), j CPU(s), j p(s), j compile(s), n * j parses
  /// Ex: compile_1(1=p,...,n), compile_2(1,2=p,...,n),...,
  /// compile_j(1,...,p=j,...,n)
  CPUCount,

};

class CompilationOptions final {
public:
  /// Indicates whether this Compilation should continue execution of subtasks
  /// even if they returned an error status.
  bool continueBuildingAfterErrors = false;

  /// Indicates whether tasks should only be executed if their output is out
  /// of date.
  bool enableIncrementalBuild;

  /// Indicates whether groups of parallel frontend jobs should be merged
  /// together and run in composite "batch jobs" when possible, to reduce
  /// redundant work.
  bool enableBatchMode = false;

  /// When true, dumps information on how long each compilation task took to
  /// execute.
  bool showDriverTimeCompilation;

  /// When true, dumps information about why files are being scheduled to be
  /// rebuilt.
  bool showIncrementalBuildDecisions = false;

  /// When true, traces the lifecycle of each driver job. Provides finer
  /// detail than ShowIncrementalBuildDecisions.
  bool showJobLifecycle = false;

  /// When true, some frontend job has requested permission to pass
  /// -emit-loaded-module-trace, so no other job needs to do it.
  bool passedEmitLoadedModuleTraceToFrontendJob = false;

  /// True if temporary files should not be deleted.
  bool saveTempFiles = false;

  /// Because each frontend job outputs the same info in its .d file, only do it
  /// on the first job that actually runs. Write out dummies for the rest of the
  /// jobs. This hack saves a lot of time in the build system when incrementally
  /// building a project with many files. Record if a scheduled job has already
  /// added -emit-dependency-path.
  bool haveAlreadyAddedDependencyPath = false;

  bool onlyOneDependencyFile = false;
  bool verifyFineGrainedDependencyGraphAfterEveryImport = false;
  bool emitFineGrainedDependencyDotFileAfterEveryImport = false;
  bool enableCrossModuleIncrementalBuild = false;

public:
  /// The output file type which should be used for the
  /// compile-job-constructions.
  file::Type outputFileType = file::Type::None;

  /// The driver link mode
  LinkMode linkMode = LinkMode::None;

  bool ShouldLink() const { return linkMode != LinkMode::None; }

  LTOKind ltoVariant = LTOKind::None;
  bool WithLTO() const { return ltoVariant != LTOKind::None; }

  std::string libLTOPath;

  /// The kind of compilation
  CompilationKind compilationKind = CompilationKind::Normal;

  /// The number of threads for multi-threaded compilation.
  unsigned numThreads = 0;

  /// Returns true if multi-threading is enabled.
  bool IsMultiThreading() const { return numThreads > 0; }

  /// The path to the SDK against which to build.
  /// (If empty, this implies no SDK.)
  std::string sdkPath;

  // /// Whether or not the output should contain debug info.
  // // FIXME: Eventually this should be replaced by dSYM generation.
  // CodeGenDebugInfoLevel debugInfoLevel = CodeGenDebugInfoLevel::None;

  // /// What kind of debug info to generate.
  // CodeGenDebugInfoFormat debugInfoFormat = CodeGenDebugInfoFormat::None;

  /// DWARF output format version number.
  // std::optional<uint8_t> dwarfVersion;

  /// The name of the module which we are building.
  std::string moduleName;

  /// Whether or not the driver should generate a module.
  bool shouldGenerateModule = false;

  /// Whether or not the driver should treat a generated module as a top-level
  /// output.
  bool shouldTreatModuleAsTopLevelOutput = false;

public:
};

} // namespace stone

#endif