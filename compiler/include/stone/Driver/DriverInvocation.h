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
#include "stone/Driver/CompilationOptions.h"

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
  CompilationKind compilationKind = CompilationKind::Normal;

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

  // void SetTargetTriple(llvm::StringRef triple);

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
  Status ParseArgs(llvm::ArrayRef<const char *> args);

private:
  Status TranslateInputArgList(const llvm::opt::InputArgList &inputArgList);

  Status ParseDriverOptions(const llvm::opt::ArgList &argList);
  Status ParseCompilationOptions(const llvm::opt::ArgList &argList);

  void ComputeLinkMode(const llvm::opt::ArgList &argList);
  Status ParseToolChainKind(const llvm::opt::ArgList &argList);
  Status ComputeCompilationKind(const llvm::opt::ArgList &argList);

  /// Construct the list of inputs and their types from the given arguments.
  ///
  /// \param args The input arguments.
  /// \param[out] Inputs The list in which to store the resulting compilation
  /// inputs.
  Status BuildInputFiles(const llvm::opt::ArgList &argList,
                         InputFileList &inputFiles);

  Status ParseOutputFileType(const llvm::opt::ArgList &argList);

public:
  void ForEachInputFile(std::function<void(InputFile &input)> callback);

  bool ShouldCompile() const { return GetAction().CanCompile(); }
  bool ShouldLink() const { return (GetLinkMode() != LinkMode::None); }

  bool IsCompileOnly() const { return (ShouldCompile() && !ShouldLink()); }
  bool IsLinkOnly() const { return (ShouldLink() && !ShouldCompile()); }

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