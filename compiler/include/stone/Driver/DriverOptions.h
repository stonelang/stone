#ifndef STONE_DRIVER_DRIVER_OPTIONS_H
#define STONE_DRIVER_DRIVER_OPTIONS_H

#include "stone/Basic/CodeGenOptions.h"
#include "stone/Basic/File.h"
#include "stone/Basic/STDAlias.h"
#include "stone/Option/Action.h"
#include "stone/Option/Options.h"

#include "llvm/ADT/Triple.h"

namespace stone {
class Driver;
class DriverOptions;
class DiagnosticEngine;


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

/// This mode controls the manner in with compile is invoked.
/// p := -primary-file
enum class CompileInvocationMode : UInt8 {
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

class DriverInputFile final {
public:
  DriverInputFile();
};
class DriverInputsAndOutputs final {

public:
  DriverInputsAndOutputs();

public:
  bool HasInputs();
};

class DriverInputsConverter final {
public:
  DriverInputsConverter(const llvm::opt::ArgList &args,
                        DriverOptions &driverOpts, DiagnosticEngine &de);
};

/// TODO: The things that are computed should be private
class DriverOptions final {
  friend Driver;
  friend DriverInputsAndOutputs;
  friend DriverInputsConverter;

private:
  /// The main action requested or computed.
  Action action;

  /// Default target triple.
  std::string defaultTargetTriple;

  /// Default target triple.
  llvm::Optional<llvm::Triple> targetVariant;

  /// The path the executing program
  llvm::StringRef mainExecutablePath;

  /// The name of the executing program
  llvm::StringRef mainExecutableName;

  /// The tool chain to use for this compilation
  ToolChainKind toolChainKind = ToolChainKind::None;

  /// The link mode computed by the driver
  LinkMode linkMode = LinkMode::None;

  /// The inputs and the associated outputs
  DriverInputsAndOutputs inputsAndOutputs;

  llvm::StringRef workingDirectory;

private:
  /// The number of threads for multi-threaded compilation.
  unsigned numThreads = 0;

  /// The method of invocating compile
  CompileInvocationMode compileInvocationMode = CompileInvocationMode::Normal;

  /// The output file type which should be used for the
  /// compile-job-constructions.
  file::FileType outputFileType = file::FileType::None;

public:
  /// Indicates whether the driver should check that the input file exist.
  bool checkInputFileExistence = false;

  /// Extra args to pass to the driver executable
  llvm::SmallVector<std::string, 2> extraMainExecutableArgs;

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

  /// The input files provided for compilation
  InputFileList inputFiles;

  /// The input file type for compilation
  file::FileType inputFileType = file::FileType::None;

  LTOKind ltoVariant = LTOKind::None;

public:
  std::string libLTOPath;

  /// The path to the SDK against which to build.
  /// (If empty, this implies no SDK.)
  std::string sdkPath;

  // /// Whether or not the output should contain debug info.
  // // FIXME: Eventually this should be replaced by dSYM generation.
  // CodeGenDebugInfoLevel debugInfoLevel = CodeGenDebugInfoLevel::None;

  // /// What kind of debug info to generate.
  // CodeGenDebugInfoFormat debugInfoFormat = CodeGenDebugInfoFormat::None;

  /// DWARF output format version number.
  // TODO: clang 17 std::optional<uint8_t> dwarfVersion;

  /// The name of the module which we are building.
  std::string moduleName;

  /// Whether or not the driver should generate a module.
  bool shouldGenerateModule = false;

  /// Whether or not the driver should treat a generated module as a top-level
  /// output.
  bool shouldTreatModuleAsTopLevelOutput = false;

public:
  /// \check that there exist a working directory
  bool HasWorkingDirectory() const { return !workingDirectory.empty(); }
  /// \return working directory for the compilation
  llvm::StringRef GetWorkingDirectory() const { return workingDirectory; }

  /// \return the main executable path
  llvm::StringRef GetMainExecutablePath() const { return mainExecutablePath; }

  /// \return the main executable name
  llvm::StringRef GetMainExecutableName() const { return mainExecutableName; }

  /// \check that there exist a tool chain kind
  bool HasToolChainKind() const { return toolChainKind != ToolChainKind::None; }
  /// \return the tool chain kind computed
  ToolChainKind GetToolChainKind() const { return toolChainKind; }

  /// \return the compile invocation mode that will be used to controll
  /// compilation
  CompileInvocationMode GetCompileInvocationMode() const {
    return compileInvocationMode;
  }

  /// Returns true if multi-threading is enabled.
  bool IsMultiThreading() const { return numThreads > 0; }

  /// Returns true LTO is suppored
  bool HasLTO() const { return ltoVariant != LTOKind::None; }

  /// \return true if there are input files.
  bool HasInputFiles() const { return !inputFiles.empty(); }

  /// \return true if there is a valid input file type
  bool HasInputFileType() const {
    return inputFileType != file::FileType::None;
  }

  /// \return the computed output file type
  file::FileType GetInputFileType() const { return inputFileType; }

  /// \return true if there exist an output file type
  bool HasOutputFileType() const {
    return (outputFileType != file::FileType::None) &&
           (file::IsOutputableFileType(outputFileType));
  }
  /// \return the computed output file type
  file::FileType GetOutputFileType() const { return outputFileType; }

  /// \return the computed input and outputs
  DriverInputsAndOutputs &GetInputsAndOutputs() { return inputsAndOutputs; }

public:
  /// \return true if the action is valid
  bool HasAction() const { return !action.IsAlien(); }

  /// \return the Action
  Action &GetAction() { return action; }

  /// \return the action
  const Action &GetAction() const { return action; }

  /// \return true if the given action only compiles without doing other
  /// compilation steps.
  bool IsCompilableAction() const;

  /// \return true if the given action only parses without doing other
  /// compilation steps.
  bool IsCompileOnlyAction() const;

  /// \return true if the given action only parses without doing other
  /// compilation steps.
  bool IsLinkableAction() const;

  /// \return true if the given action only parses without doing other
  /// compilation steps.
  bool IsLinkOnlyAction() const;

public:
  DriverOptions();
};

} // namespace stone

#endif