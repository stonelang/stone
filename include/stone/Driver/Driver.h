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

#include "stone/Driver/JobRequest.h"

namespace stone {

class JobRequest;
class TopLevelJobRequest;
class Compilation;

class HotCache final {
public:
  JobRequest *currentJobRequest;
  /// We keep track of the inputs for the module that we are building.
  /// These are generally CompileJobRequest(s)
  llvm::SmallVector<const JobRequest *, 4> moduleInputs;

  /// When are building the  request(s), keep track of the linker dependecies
  llvm::SmallVector<const JobRequest *, 2> linkerDeps;

  /// These are the top-level job requests -- we use them recursively to build
  /// out the "real" jobs.
  llvm::SmallVector<const TopLevelJobRequest *, 16> topLevelJobRequests;

public:
  bool HasModuleInputs() { return moduleInputs.size(); }
  bool HasLinkerDeps() { return linkerDeps.size(); }
  bool HasTopLevelJobRequest() { return topLevelJobRequests.size(); }
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

  /// Lifetime management.
  llvm::SmallVector<std::unique_ptr<const JobRequest>, 32> jobRequests;

public:
  Driver(const Driver &) = delete;
  void operator=(const Driver &) = delete;
  Driver() = delete;

public:
  Driver(llvm::StringRef name, llvm::StringRef path);
  ~Driver();

  void Initialize();
  void Finish();
  void PrintVersion();

public:
  template <typename T, typename... Args> T *MakeJobRequest(Args &&...args) {
    auto result = new T(std::forward<Args>(args)...);
    jobRequests.emplace_back(result);
    return result;
  }

public:
  llvm::opt::InputArgList &
  ParseArgs(llvm::ArrayRef<const char *> args) override;

  void ComputeLinkMode(const llvm::opt::InputArgList &ial);
  LinkMode GetLinkMode() const { return driverOpts.outputOptions.linkMode; }

  std::unique_ptr<llvm::opt::DerivedArgList>
  TranslateInputArgList(const llvm::opt::InputArgList &ial,
                        llvm::StringRef workDir);

  // TODO:
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

  // void BuildIntents(Compilation &compilation, CompilationHotInfo &chi,
  //                   const file::Files &inputs);
  // void PrintIntents(CompilationHotInfo &chi);

  void BuildJobRequests(Compilation &c, HotCache &hc, const file::Files &inputs,
                        const OutputOptions &outputOptions);

  void PrintJobRequests(const HotCache &hc);

  // void BuildJobs(Compilation &c, HotCache &hc);
  // void PrintJobs(CompilationHotInfo &chi);

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
    if ((driverOpts.outputOptions.linkMode == LinkMode::None) &&
        GetMode().CanCompile()) {
      return true;
    }
    return false;
  }
  bool CanCompileFile() { return file::CanCompile(driverOpts.inputFileType); }

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
