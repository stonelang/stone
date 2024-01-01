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

using ToolChainKind = llvm::Triple::OSType;

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
enum class CompileStyle : UInt8 {
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

/// Only for DriverInputFiles
class DriverInputsAndOutputs final {

  friend DriverInputsConverter;
  std::vector<const DriverInputFile *> inputs;

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

/// Only for input files convertions
class DriverInputsConverter final {

  const llvm::opt::ArgList &args;
  DriverOptions &driverOpts;
  DiagnosticEngine &diags;

  llvm::SetVector<llvm::StringRef> files;

public:
  DriverInputsConverter(const llvm::opt::ArgList &args,
                        DriverOptions &driverOpts, DiagnosticEngine &diags);

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
  DiagnosticEngine &diags;

public:
  DriverOutputsConverter(const llvm::opt::ArgList &args,
                         DriverInputsAndOutputs &inputsAndOutputs,
                         DiagnosticEngine &diags)
      : args(args), inputsAndOutputs(inputsAndOutputs), diags(diags) {}

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
  DiagnosticEngine &diags;

public:
  DriverOptionsConverter(const llvm::opt::ArgList &args,
                         DriverOptions &driverOpts, DiagnosticEngine &diags);

private:
  ToolChainKind ComputeToolChainKind();
  llvm::StringRef ComputeWorkingDirectory();
  CompileStyle ComputeCompileStyle();
  LTOKind ComputeLTO();
  LinkMode ComputeLinkMode();
  llvm::Triple ComputeTarget();

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
  CompileStyle compileStyle = CompileStyle::Normal;

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
  bool HasLinkMode() const { return linkMode != LinkMode::None; }
  /// \return the tool chain kind computed
  LinkMode GetLinkMode() const { return linkMode; }

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
  Action action;

  /// Default target triple.
  String defaultTargetTriple;

  /// Default target triple.
  llvm::Optional<llvm::Triple> targetVariant;

  /// The path the executing program
  llvm::StringRef mainExecutablePath;

  /// The name of the executing program
  llvm::StringRef mainExecutableName;

  /// The current working director
  llvm::StringRef workingDirectory;

  /// The tool chain to use for this compilation
  ToolChainKind toolChainKind = ToolChainKind::UnknownOS;

  /// The inputs and the associated outputs with files
  DriverInputsAndOutputs inputsAndOutputs;

  /// The output information used during compilation
  DriverOutputInfo driverOutputInfo;

public:
  /// \return true if the action is valid
  bool HasAction() const { return !action.IsAlien(); }

  /// \return the Action
  Action &GetAction() { return action; }

  /// \return the action
  const Action &GetAction() const { return action; }

  /// \return true if the given action only parses without doing other
  /// compilation steps.
  bool IsSupportAction() const;

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
  const DriverInputsAndOutputs &GetInputsAndOutputs() const {
    return inputsAndOutputs;
  }

  /// The output information used during compilation
  const DriverOutputInfo &GetDriverOutputInfo() const {
    return driverOutputInfo;
  }

public:
  DriverOptions();
};

} // namespace stone

#endif