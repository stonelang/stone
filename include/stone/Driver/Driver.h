#ifndef STONE_DRIVER_DRIVER_H
#define STONE_DRIVER_DRIVER_H

#include "stone/Core/LLVM.h"
#include "stone/Core/List.h"
#include "stone/Core/OutputFileMap.h"
#include "stone/Core/StatisticEngine.h"
#include "stone/Driver/BuildSystem.h"
#include "stone/Driver/DriverOptions.h"
#include "stone/Driver/ToolChain.h"
#include "stone/Option/Support.h"

namespace stone {

class Compilation;
class CompilationOutputContext final {
public:
};

class Intent;
class BuildCompilationCache final {
public:
  /// All the inputs associated with the module
  llvm::SmallVector<const Intent *, 4> moduleInputs;

  /// The top level intents -- ex: linker. We only queue the top level intents.
  llvm::SmallVector<const Intent *, 16> topLevelIntents;

  /// All the inputs for the linker
  llvm::SmallVector<const Intent *, 2> linkerInputs;

  Intent *current;
};

class Driver final {

  llvm::StringRef name;
  llvm::string path;

  opts::Support optSupport;

  /// The system context
  Context ctx;

  DriverOptions driverOpts;
  CompilationOutputContext coc;

  std::unique_ptr<ToolChain> toolChain;
  std::unique_ptr<BuildSystem> buildSystem;
  std::unique_ptr<Compilation> compilation;

  bool justLink = false;

  /// The OutputFileMap describing the Compilation's outputs, populated both by
  /// the user-provided output file map (if it exists) and inference rules that
  /// derive otherwise-unspecified output filenames from context.
  OutputFileMap outputFileMap;

  BuildCompilationCache bcc;

public:
  Driver(const Driver &) = delete;
  void operator=(const Driver &) = delete;
  Driver() = delete;

public:
  Driver(llvm::StringRef name, llvm::StringRef path,
         CompilationListener *listener = nullptr);
  ~Driver();

  void Initialize();
  void Finish();

public:
  void Run();
  void PrintHelp();
  void PrintVersion();

public:
  // TODO: May just want parse to return the ial
  bool ParseArgs(llvm::ArrayRef<const char *> args);
  llvm::opt::InputArgList &GetGetInputArgList() {
    return optSupport.GetInputArgList();
  }

  Mode &GetMode() { return optSupport.GetMode(); }
  const Mode &GetMode() const { return optSupport.GetMode(); }

  void ComputeLinkMode();
  LinkMode GetLinkMode() const { return driverOpts.linkMode; }

  bool CanLink() const { return (GetLinkMode() != LinkMode::None); }
  bool JustLink() const { return justLink; }

  std::unique_ptr<ToolChain> BuildToolChain(const llvm::opt::InputArgList &ial);

  void BuildOutputContext();

public:
  // void BuildOutputContext();
  std::unique_ptr<Compilation> BuildCompilation(ToolChain &tc);

  stone::CompileModel ComputeCompileModel(const llvm::opt::DerivedArgList &args,
                                          const file::Files &inputs) const;
  CompileModel GetCompileModel() const { return driverOpts.compileModelKind; }

  void BuildIntents(BuildCompilationCache &bcc);
  void PrintIntents(BuildCompilationCache &bcc);

  void BuildJobs(BuildCompilationCache &bcc);
  void PrintJobs(BuildCompilationCache &bcc);

  BuildCompilationCache &GetBuildCompilationCache() { return bcc; }
  CompilationOutputContext &GetOuputContext() { return coc; }

public:
  opts::Support &GetOptSupport() { return optSupport; }

  file::Type GetInputFileType() const { return driverOpts.inputFileType; }
  file::Type GetOutputFileType() const { return driverOpts.outputFileType; }

  DriverOptions &GetDriverOptions() { return driverOpts; }
  const DriverOptions &GetDriverOptions() const { return driverOpts; }
  SessionOptions &GetOptions() override { return driverOpts; }

  OutputFileMap &GetOutputFileMap() { return outputFileMap; }
  const OutputFileMap &GetOutputFileMap() const { return outputFileMap; }

  bool JustCompile() const { return driverOpts.linkMode == LinkMode::None; }
  bool CanCompile() { return file::CanCompile(driverOpts.inputFileType); }

  Compilation &GetCompilation() { return *compilation.get(); }

  BuildSystem &GetBuildSystem() { return *buildSystem.get(); }

  // void BuildImageBaseName(const LinkJob &linkJob, ImageBaseName
  // &imageBaseName);

  llvm::StringRef ComputeOutputFilename(
      // const JobAction *JA,
      // const TypeToPathMap *OutputMap,
      // StringRef workingDirectory,
      // bool AtTopLevel,
      // StringRef BaseInput,
      // StringRef PrimaryInput,
      // llvm::SmallString<128> &Buffer
  );

public:
  // IntentExecutor ConstructIntentExecutor(ProcessIntent& intent);
  // IntentExecutor ConstructIntentExecutor(CompilationIntent &intent);
  // void BuildOptions() override;
};

} // namespace stone

#endif
