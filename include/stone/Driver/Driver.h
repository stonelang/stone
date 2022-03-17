#ifndef STONE_DRIVER_DRIVER_H
#define STONE_DRIVER_DRIVER_H

#include "stone/Core/LLVM.h"
#include "stone/Core/List.h"
#include "stone/Core/OutputFileMap.h"
#include "stone/Core/StatisticEngine.h"
#include "stone/Driver/BuildSystem.h"
#include "stone/Driver/CompilationListener.h"
#include "stone/Driver/DriverOptions.h"
#include "stone/Driver/ToolChain.h"
#include "stone/Session/Session.h"

namespace stone {

class Intent;
class Compilation;

class CompilationHotInfo final {
public:
  /// All the inputs associated with the module
  llvm::SmallVector<const Intent *, 4> moduleInputs;

  /// The top level intents -- ex: linker. We only queue the top level intents.
  llvm::SmallVector<const Intent *, 16> topLevelIntents;

  /// All the inputs for the linker
  llvm::SmallVector<const Intent *, 2> linkerInputs;

  Intent *current;
};

class Driver final : public Session {

  llvm::StringRef name;
  llvm::StringRef path;

  DriverOptions driverOpts;

  std::unique_ptr<ToolChain> toolChain;
  std::unique_ptr<BuildSystem> buildSystem;
  std::unique_ptr<Compilation> compilation;

  bool justLink = false;

  /// The OutputFileMap describing the Compilation's outputs, populated both by
  /// the user-provided output file map (if it exists) and inference rules that
  /// derive otherwise-unspecified output filenames from context.
  OutputFileMap outputFileMap;

  CompilationListener *listener = nullptr;

public:
  Driver(const Driver &) = delete;
  void operator=(const Driver &) = delete;
  Driver() = delete;

public:
  Driver(llvm::StringRef name, llvm::StringRef path);
  ~Driver();

  void Initialize();
  void Finish();

public:
  void PrintVersion();

public:
  llvm::opt::InputArgList &
  ParseArgs(llvm::ArrayRef<const char *> args) override;

  void ComputeLinkMode(const llvm::opt::InputArgList &ial);
  LinkMode GetLinkMode() const { return driverOpts.outputOptions.linkMode; }

  std::unique_ptr<llvm::opt::DerivedArgList>
  TranslateInputArgList(const llvm::opt::InputArgList &ial,
                        llvm::StringRef workDir);

  bool CanLink() const { return (GetLinkMode() != LinkMode::None); }
  bool JustLink() const { return justLink; }

  void ComputeOptions(const llvm::opt::InputArgList &ial);

  void ComputeOutputOptions(const ToolChain &toolChain,
                            const llvm::opt::InputArgList &ial,
                            const file::Files &inputs,
                            OutputOptions &outputOptions);

  CompilingModel ComputeCompilingModel(const llvm::opt::DerivedArgList &dal,
                                       bool &isBatchModel) const;

  CompilingModel GetCompilingModel() const {
    return driverOpts.outputOptions.compilingModel;
  }

public:
  std::unique_ptr<ToolChain> BuildToolChain(const llvm::opt::InputArgList &ial);
  std::unique_ptr<Compilation> BuildCompilation(ToolChain &toolChain,
                                                llvm::opt::InputArgList &ial);

  void BuildIntents(Compilation &compilation, CompilationHotInfo &chi,
                    const file::Files &inputs);
  void PrintIntents(CompilationHotInfo &chi);

  void BuildJobs(Compilation &compilation, CompilationHotInfo &chi);
  void PrintJobs(CompilationHotInfo &chi);

public:
  BaseOptions &GetBaseOptions() override { return driverOpts; }
  file::Type GetInputFileType() const { return driverOpts.inputFileType; }
  file::Type GetOutputFileType() const {
    return driverOpts.outputOptions.outputFileType;
  }

  DriverOptions &GetDriverOptions() { return driverOpts; }
  const DriverOptions &GetDriverOptions() const { return driverOpts; }

  OutputFileMap &GetOutputFileMap() { return outputFileMap; }
  const OutputFileMap &GetOutputFileMap() const { return outputFileMap; }

  bool JustCompile() const {
    return driverOpts.outputOptions.linkMode == LinkMode::None;
  }
  bool CanCompile() { return file::CanCompile(driverOpts.inputFileType); }

  Compilation &GetCompilation() { return *compilation.get(); }
  BuildSystem &GetBuildSystem() { return *buildSystem.get(); }

  CompilationListener *GetListener() { return listener; }
  void SetListener(CompilationListener *l) { listener = l; }

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
};

} // namespace stone

#endif
