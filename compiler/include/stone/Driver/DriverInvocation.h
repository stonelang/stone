#ifndef STONE_DRIVER_DRIVER_INVOCATION_H
#define STONE_DRIVER_DRIVER_INVOCATION_H

#include "stone/Basic/CodeGenOptions.h"
#include "stone/Basic/FileMgr.h"
#include "stone/Basic/FileSystemOptions.h"
#include "stone/Basic/LangOptions.h"
#include "stone/Basic/STDAlias.h"
#include "stone/Basic/Status.h"
#include "stone/Diag/DiagnosticEngine.h"
#include "stone/Diag/DiagnosticOptions.h"
#include "stone/Diag/DriverDiagnostic.h"
#include "stone/Option/Action.h"
#include "stone/Option/Options.h"

#include "llvm/Option/Arg.h"
#include "llvm/Option/ArgList.h"

using namespace llvm::opt;

namespace stone {

class DriverInvocation;
class JobOutput;

// using  ToolChainPlatform = llvm::Triple::OSType;

enum class ToolChainKind {
  None = 0,
  /// Darwin tool-chain
  Darwin,
  /// Linux tool-chain
  Linux,
  /// Windows tool-chain
  Windows,
  /// FreeBSD tool-chain
  FreeBSD,
  /// OpenBSD tool-chain
  OpenBSD,
  /// Android tool-chain
  Android,
  /// Any unix tool-chain
  Unix
};

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
  Quadratic = 0,
  /// n input(s), n compile(s), n parses
  /// Ex: compile_1(1=p), compile_2(2=p),..., compile_n(n=p)
  Flat,
  /// n inputv(s), j CPU(s), j compile(s), n * j parses
  /// Ex: compile_1(1=p,...,n),...,
  /// compile_2(1,2=p,...,n),...,compile_j(1,...,p=j,...,n)
  CPUCount,
  /// p := 0, n inputs, 1 compile, n parses
  /// Ex: compile(1,....,n)
  Single,
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
};

class DriverOptions final {
public:
  /// The main action requested or computed.
  Action mainAction;

  /// Default target triple.
  std::string defaultTargetTriple;

  /// Default target triple.
  llvm::Optional<llvm::Triple> targetVariant;

  /// The path the executing program
  llvm::StringRef mainExecutablePath;

  /// The name of the executing program
  llvm::StringRef mainExecutableName;

  /// Extra args to pass to the driver executable
  llvm::SmallVector<std::string, 2> extraMainExecutableArgs;

public:
  /// Outputs

  /// The driver link mode
  LinkMode linkMode = LinkMode::None;

  bool ShouldLink() const { return linkMode != LinkMode::None; }

  LTOKind ltoVariant = LTOKind::None;
  bool WithLTO() const { return ltoVariant != LTOKind::None; }

  std::string libLTOPath;

  /// The kind of compilation
  CompilationKind compilationKind = CompilationKind::Quadratic;

  /// The number of threads for multi-threaded compilation.
  unsigned numThreads = 0;

  /// Returns true if multi-threading is enabled.
  bool IsMultiThreading() const { return numThreads > 0; }

  /// The path to the SDK against which to build.
  /// (If empty, this implies no SDK.)
  std::string sdkPath;

  /// The output file type which should be used for the
  /// compile-job-constructions.
  file::Type outputFileType = file::Type::None;

  /// The file input type
  file::Type inputFileType = file::Type::None;

  InputFileList inputFiles;

  /// Indicates whether the driver should check that the input file exist.
  bool checkInputFileExistence = false;

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

  /// Print the jobs in the TaskQueue
  bool printJobs = false;

  ToolChainKind toolChainKind = ToolChainKind::None;

public:
  std::string toolsDirectory;

public:
  DriverOptions();
};

class DriverInvocation final {

  SrcMgr srcMgr;
  DiagnosticEngine diags{srcMgr};

  DriverOptions driverOpts;
  CompilationOptions compilationOpts;
  DiagnosticOptions diagOpts;
  LangOptions langOpts;
  FileSystemOptions fileSystemOpts;

  std::unique_ptr<llvm::opt::OptTable> optTable;
  std::unique_ptr<llvm::opt::InputArgList> inputArgList;
  std::unique_ptr<llvm::opt::DerivedArgList> derivedArgList;

public:
  DriverInvocation();

public:
  DriverOptions &GetDriverOptions() { return driverOpts; }
  const DriverOptions &GetDriverOptions() const { return driverOpts; }

  CompilationOptions &GetCompilationOptions() { return compilationOpts; }
  const CompilationOptions &GetCompilationOptions() const {
    return compilationOpts;
  }

  LangOptions &GetLangOptions() { return langOpts; }
  const LangOptions &GetLangOptions() const { return langOpts; }

  FileSystemOptions &GetFileSystemOptions() { return fileSystemOpts; }
  const FileSystemOptions &GetFileSystemOptions() const {
    return fileSystemOpts;
  }

  bool HasAction() { return !driverOpts.mainAction.IsAlien(); }
  Action &GetAction() { return driverOpts.mainAction; }
  const Action &GetAction() const { return driverOpts.mainAction; }

  LinkMode GetLinkMode() const { return driverOpts.linkMode; }
  ToolChainKind GetToolChainKind() const { return driverOpts.toolChainKind; }
  CompilationKind GetCompilationKind() const {
    return driverOpts.compilationKind;
  }

  void SetTargetTriple(llvm::StringRef triple);

  void SetMainExecutablePath(llvm::StringRef mainExecutablePath) {
    driverOpts.mainExecutablePath = mainExecutablePath;
  }
  void SetMainExecutableName(llvm::StringRef mainExecutableName) {
    driverOpts.mainExecutablePath = mainExecutableName;
  }

public:
  DiagnosticEngine &GetDiags() { return diags; }
  DiagnosticOptions &GetDiagnosticOptions() { return diagOpts; }
  const DiagnosticOptions &GetDiagnosticOptions() const { return diagOpts; }
  void AddDiagnosticConsumer(DiagnosticConsumer &consumer) {
    diags.AddConsumer(consumer);
  }
  void RemoveDiagnosticConsumer(DiagnosticConsumer &consumer) {
    diags.RemoveConsumer(consumer);
  }

public:
  llvm::opt::OptTable &GetOptTable() { return *optTable; }
  const llvm::opt::OptTable &GetOptTable() const { return *optTable; }

  llvm::opt::InputArgList &GetInputArgList() { return *inputArgList; }
  llvm::opt::DerivedArgList &GetDerivedArgList() { return *derivedArgList; }

public:
  Status ParseCommandLine(llvm::ArrayRef<const char *> args);
  Status TranslateInputArgList(const llvm::opt::InputArgList &inputArgList);

  Status ParseDriverOptions(const llvm::opt::ArgList &argList);
  Status ParseCompilationOptions(const llvm::opt::ArgList &argList);

  void ComputeLinkMode(const llvm::opt::ArgList &argList);
  Status ParseToolChainKind(const llvm::opt::ArgList &argList);
  Status ComputeCompilationKind(const llvm::opt::ArgList &argList);

  Status ComputeInputFiles(const llvm::opt::ArgList &argList);

  /// Construct the list of inputs and their types from the given arguments.
  ///
  /// \param args The input arguments.
  /// \param[out] Inputs The list in which to store the resulting compilation
  /// inputs.
  Status BuildInputFiles(const llvm::opt::ArgList &argList,
                         InputFileList &inputFiles) const;
  void ForEachInputFile(std::function<void(InputFile &input)> callback);

  bool IsCompilable() const { return GetAction().CanCompile(); }
  bool IsLinkable() const { return (GetLinkMode() != LinkMode::None); }

  bool IsCompileOnly() const { return (IsCompilable() && !IsLinkable()); }
  bool IsLinkOnly() const { return (IsLinkable() && !IsCompilable()); }

public:
  /// Might this sort of compile have explicit primary inputs?
  /// When running a single compile for the whole module (in other words
  /// "whole-module-optimization" mode) there must be no -primary-input's and
  /// nothing in a (preferably non-existent) -primary-filelist. Left to its own
  /// devices, the driver would forget to omit the primary input files, so
  /// return a flag here.
  Status MightHaveExplicitPrimaryInputs(const JobOutput &jobOutput) const;
};

} // namespace stone

#endif