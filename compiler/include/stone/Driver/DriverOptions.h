#ifndef STONE_DRIVER_DRIVER_OPTIONS_H
#define STONE_DRIVER_DRIVER_OPTIONS_H

#include "stone/Basic/CodeGenOptions.h"
#include "stone/Basic/LangOptions.h"
#include "stone/Basic/STDAlias.h"
#include "stone/Basic/Status.h"
#include "stone/Driver/DriverInputFile.h"
#include "stone/Option/Action.h"
#include "stone/Option/Options.h"

#include "llvm/ADT/SetVector.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/Triple.h"

namespace stone {

class Driver;
class CommandOutput;
class DriverOptions;
class DriverInputFile;
class DiagnosticEngine;
class DriverInputsAndOutputs;
class DriverInputsConverter;

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
  /// n input(s), n compile(s), n parses
  /// Ex: compile_1(1=p), compile_2(2=p),..., compile_n(n=p)
  Multiple = 0,
  /// n inputs, 1 compile, n parses, p := 0
  /// Ex: compile(1,....,n)
  Single,
  /// Batch
  Batch,
};

enum class ToolChainKind {
  None = 0,
  // We always default to unix
  Unix,
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
};

/// Only for DriverInputFiles
class DriverInputsAndOutputs final {

  friend DriverInputsConverter;
  std::vector<const DriverInputFile *> inputs;

  /// Punt where needed to enable batch mode experiments.
  bool areBatchModeChecksBypassed = false;

  /// Recover missing inputs. Note that recovery itself is users responsibility.
  bool shouldRecoverMissingInputs = false;

public:
  DriverInputsAndOutputs() = default;
  DriverInputsAndOutputs(const DriverInputsAndOutputs &other);
  DriverInputsAndOutputs &operator=(const DriverInputsAndOutputs &other);

public:
  llvm::ArrayRef<const DriverInputFile *> GetInputs() const { return inputs; }
  std::vector<std::string> GetDriverInputFilenames() const;

  unsigned InputCount() const { return inputs.size(); }
  bool HasInputs() const { return !inputs.empty() && (InputCount() > 0); }
  bool HasNoInputs() const { return !HasInputs(); }
  bool HasSingleInput() const { return InputCount() == 1; }

  bool ShouldRecoverMissingInputs() { return shouldRecoverMissingInputs; }
  void SetShouldRecoverMissingInputs() { shouldRecoverMissingInputs = true; }

  const DriverInputFile *FirstInput() const {
    assert(HasInputs());
    return inputs.front();
  }

  const DriverInputFile *LastInput() const { return inputs.back(); }
  /// If \p fn returns true, exits early and returns true.
  void ForEachInput(std::function<void(const DriverInputFile *)> fn) const;

public:
  void ClearInputs();
  void AddInput(const DriverInputFile *input);
  void AddInput(llvm::StringRef file);
};

class DriverArgList final {
  const llvm::opt::ArgList &args;

public:
  DriverArgList(const llvm::opt::ArgList &args);

public:
  const llvm::opt::ArgList &GetArgs() const;
  bool HasLTO() const;
  const llvm::opt::Arg *GetLTO() const;

  bool HasLTOLibray() const;
  const llvm::opt::Arg *GetLTOLibrary() const;

  unsigned GetNumThreads() const;
};
/// Only for input files convertions
class DriverInputsConverter final {

  llvm::SetVector<llvm::StringRef> files;
  const llvm::opt::ArgList &args;
  DriverOptions &driverOpts;
  Driver &driver;

public:
  DriverInputsConverter(const llvm::opt::ArgList &args,
                        DriverOptions &driverOpts, Driver &driver);

public:
  /// Produces a CompilerInputsAndOutputs object with the inputs populated from
  /// the arguments the converter was initialized with.
  ///
  /// \param buffers If present, buffers read in the processing of the
  /// invocation inputs will be saved here. These should only be used for
  /// debugging purposes.
  llvm::Optional<DriverInputsAndOutputs> Convert();

private:
  Status ReadFilesFromCommandLine();
  Status AddFile(llvm::StringRef file);

  /// Returns the newly set-up DriverInputsAndOutputs
  llvm::Optional<DriverInputsAndOutputs> CreateInputFiles();

public:
};

/// Only for output files conversion
class DriverOutputsConverter final {
  const llvm::opt::ArgList &args;
  DriverInputsAndOutputs &inputsAndOutputs;
  Driver &driver;

public:
  DriverOutputsConverter(const llvm::opt::ArgList &args,
                         DriverInputsAndOutputs &inputsAndOutputs,
                         Driver &driver)
      : args(args), inputsAndOutputs(inputsAndOutputs), driver(driver) {}

  // bool Convert(std::vector<std::string> &mainOutputs,
  //              std::vector<std::string> &mainOutputsForIndexUnits,
  //              std::vector<SupplementaryOutputPaths> &supplementaryOutputs,
  //              const Action &action);

  /// Try to read an output file list file.
  /// \returns `None` if it could not open the filelist.
  // static Optional<std::vector<std::string>>
  // ReadOutputFileList(StringRef filelistPath, DiagnosticEngine &de);
};

/// For all driver options
class DriverOptionsConverter final {
  const llvm::opt::ArgList &args;
  DriverOptions &driverOpts;
  Driver &driver;

public:
  DriverOptionsConverter(const llvm::opt::ArgList &args,
                         DriverOptions &driverOpts, Driver &driver);

private:
  ToolChainKind ComputeToolChainKind();
  llvm::StringRef ComputeWorkingDirectory();
  CompileInvocationMode ComputeCompileInvocationMode();
  LTOKind ComputeLTO();
  LinkMode ComputeLinkMode();
  void SetTriple(llvm::Triple triple);
  void ComputeOutputInfo();
  void ComputeGeneratePCH();

public:
  Status Convert();
};

class DriverOutputInfo final {

  friend Driver;
  friend DriverInputsAndOutputs;
  friend DriverInputsConverter;
  friend DriverOptionsConverter;

  /// The number of threads for multi-threaded compilation.
  unsigned numThreads = 0;

  /// The mode in which the driver should invoke the frontend.
  CompileInvocationMode compileInvocationMode = CompileInvocationMode::Multiple;

  // llvm::Optional<MSVCRuntime> msvRuntimeVariant = llvm::None;

  /// The output type which should be used for compile job constructions.
  file::FileType outputFileType = file::FileType::None;

  /// Describes if and how the output of compile actions should be
  /// linked together.
  LinkMode linkMode = LinkMode::None;

  LTOKind ltoVariant = LTOKind::None;

  String libLTOPath;

  /// The name of the module which we are building.
  String moduleName;

  /// The path to the SDK against which to build.
  /// (If empty, this implies no SDK.)
  String sdkPath;

  CompilationOutputLevel compilationOutputLevel =
      CompilationOutputLevel::Normal;

public:
  /// Whether or not the output should contain debug info.
  // FIXME: Eventually this should be replaced by dSYM generation.
  // IRGenDebugInfoLevel DebugInfoLevel = IRGenDebugInfoLevel::None;

  /// What kind of debug info to generate.
  // IRGenDebugInfoFormat DebugInfoFormat = IRGenDebugInfoFormat::None;

  /// DWARF output format version number.
  // std::optional<uint8_t> DWARFVersion;

  /// Whether or not the driver should generate a module.
  bool shouldGenerateModule = false;

  /// Whether or not the driver should treat a generated module as a top-level
  /// output.
  bool shouldTreatModuleAsTopLevelOutput = false;

  /// Whether the compiler picked the current module name, rather than the user.
  bool moduleNameIsFallback = false;

  // stone::OptionSet<SanitizerKind> selectedSanitizers;

public:
  /// Returns true if multi-threading is enabled.
  bool IsMultiThreading() const { return numThreads > 0; }

  /// \return true if there exist an output file type
  bool HasOutputFileType() const {
    return (outputFileType != file::FileType::None) &&
           (file::IsOutputableFileType(outputFileType));
  }
  /// \return the computed output file type
  file::FileType GetOutputFileType() const { return outputFileType; }

  /// \check that there exist a tool chain kind
  bool ShouldLink() const { return linkMode != LinkMode::None; }

  /// \return the tool chain kind computed
  LinkMode GetLinkMode() const { return linkMode; }

  bool IsStaticLibraryLink() const {
    return linkMode == LinkMode::StaticLibrary;
  }
  bool IsDynamicLibraryLink() const {
    return linkMode == LinkMode::DynamicLibrary;
  }
  bool IsExecutableLink() const { return linkMode == LinkMode::Executable; }

  String GetModuleNmae() { return moduleName; }

  String GetSDKPath() { return sdkPath; }

  /// Returns true LTO is suppored
  bool HasLTO() const { return ltoVariant != LTOKind::None; }
  /// Returns true LTO is suppored
  LTOKind GetLTO() const { return ltoVariant; }

  /// \return the compilation output level
  CompilationOutputLevel GetCompilationOutputLevel() const {
    return compilationOutputLevel;
  }

  bool ShouldGenerateModule() const { return shouldGenerateModule; }

  bool IsMultipleCompileInvocation() const {
    return compileInvocationMode == CompileInvocationMode::Multiple;
  }
  bool IsSingleCompileInvocation() const {
    return compileInvocationMode == CompileInvocationMode::Single;
  }
  bool IsBatchCompileInvocation() const {
    return compileInvocationMode == CompileInvocationMode::Batch;
  }

  CompileInvocationMode GetCompileInvocationMode() const {
    return compileInvocationMode;
  }

public:
  /// Might this sort of compile have explicit primary inputs?
  /// When running a single compile for the whole module (in other words
  /// "whole-module-optimization" mode) there must be no -primary-input's and
  /// nothing in a (preferably non-existent) -primary-filelist. Left to its own
  /// devices, the driver would forget to omit the primary input files, so
  /// return a flag here.
  bool MightHaveExplicitPrimaryInputs(const CommandOutput &commandOutput) const;
};

class DriverOptions final {

  friend Driver;
  friend DriverInputsAndOutputs;
  friend DriverInputsConverter;
  friend DriverOptionsConverter;

  /// The main action requested or computed.
  Action mainAction;

  /// Default target triple.
  String defaultTargetTriple;

  llvm::Triple triple;

  /// Default target triple.
  llvm::Optional<llvm::Triple> targetVariant;

  /// The path the executing program
  llvm::StringRef mainExecutablePath;

  /// The name of the executing program
  llvm::StringRef mainExecutableName;

  /// The current working director
  llvm::StringRef workingDirectory;

  /// The tool chain to use for this compilation
  ToolChainKind toolChainKind = ToolChainKind::None;

  /// The inputs and the associated outputs with files
  DriverInputsAndOutputs inputsAndOutputs;

  /// The output information used during compilation
  DriverOutputInfo driverOutputInfo;

public:
  /// The input file type for compilation
  file::FileType inputFileType = file::FileType::None;

  bool shouldProcessDuplicateInputFile = false;

  bool allowModuleWithCompilerErrors = false;

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

  bool shouldGeneratePCH = false;

public:
  /// \check that there exist a working directory
  bool HasWorkingDirectory() const {
    return !workingDirectory.empty() && workingDirectory.size() > 0;
  }
  /// \return working directory for the compilation
  llvm::StringRef GetWorkingDirectory() const { return workingDirectory; }

  /// \return the main executable path
  llvm::StringRef GetMainExecutablePath() const { return mainExecutablePath; }

  /// \check that there exist the main executable path
  bool HasMainExecutablePath() const {
    return !mainExecutablePath.empty() && mainExecutablePath.size() > 0;
  }
  /// \return the main executable name
  llvm::StringRef GetMainExecutableName() const { return mainExecutableName; }

  /// \check that there exist the main executable path
  bool HasMainExecutableName() const {
    return !mainExecutableName.empty() && mainExecutableName.size() > 0;
  }

  /// \return true if the action is valid
  bool HasMainAction() const { return !mainAction.IsAlien(); }

  /// \return the Action
  Action &GetMainAction() { return mainAction; }

  /// \return the action
  const Action &GetMainAction() const { return mainAction; }

  /// \check that there exist a tool chain kind
  bool HasToolChainKind() const { return toolChainKind != ToolChainKind::None; }
  /// \return the tool chain kind computed
  ToolChainKind GetToolChainKind() const { return toolChainKind; }

  const llvm::Triple &GetTriple() const { return triple; }

  /// \return true if it is the help action
  bool IsHelpAction() const;

  /// \return true it is the help hidden action
  bool IsHelpHiddenAction() const;

  /// \return true if it is the print version
  bool IsPrintVersionAction() const;

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

  /// \return the computed input and outputs
  DriverInputsAndOutputs &GetInputsAndOutputs() { return inputsAndOutputs; }
  /// \return the computed input and outputs
  const DriverInputsAndOutputs &GetInputsAndOutputs() const {
    return inputsAndOutputs;
  }

  /// The output information used during compilation
  const DriverOutputInfo &GetDriverOutputInfo() const {
    return driverOutputInfo;
  }

  /// \return true if there is a valid input file type
  bool HasInputFileType() const {
    return (inputFileType != file::FileType::None &&
            inputFileType != file::FileType::Image);
  }

  /// \return the computed output file type
  file::FileType GetInputFileType() const { return inputFileType; }

  bool IsNoneInputFileType() const {
    return file::IsNoneFileType(inputFileType);
  }
  bool IsStoneInputFileType() const {
    return file::IsStoneFileType(inputFileType);
  }
  bool IsObjectInputFileType() const {
    return file::IsObjectFileType(inputFileType);
  }
  bool IsIRInputFileType() const { return inputFileType == file::FileType::IR; }

public:
  DriverOptions();
};

} // namespace stone

#endif