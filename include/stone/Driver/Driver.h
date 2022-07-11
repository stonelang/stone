#ifndef STONE_DRIVER_DRIVER_H
#define STONE_DRIVER_DRIVER_H

#include "stone/Basic/LLVM.h"
#include "stone/Basic/List.h"
#include "stone/Basic/OutputFileMap.h"
#include "stone/Basic/StatisticEngine.h"
#include "stone/Driver/BuildSystem.h"
#include "stone/Driver/CompilationListener.h"
#include "stone/Driver/CompilationModel.h"
#include "stone/Driver/DriverOptions.h"
#include "stone/Driver/Intent.h"
#include "stone/Driver/ToolChain.h"
#include "stone/Session/Session.h"

namespace stone {

class Job;
class TaskQueue;

class Compilation;

class HotCache final {
  IntentCache intentCache;
  JobCache jobCache;

public:
  IntentCache &GetIntentCache() { return intentCache; }
  JobCache &GetJobCache() { return jobCache; }
};

class Driver final : public Session {

  std::unique_ptr<DriverOptions> driverOpts;

  std::unique_ptr<ToolChain> toolChain;
  std::unique_ptr<BuildSystem> buildSystem;
  std::unique_ptr<Compilation> compilation;

  /// The OutputFileMap describing the Compilation's outputs, populated both by
  /// the user-provided output file map (if it exists) and inference rules that
  /// derive otherwise-unspecified output filenames from context.
  OutputFileMap outputFileMap;

  CompilationListener *listener = nullptr;

  /// Lifetime management.
  // llvm::SmallVector<std::function<void(Compilation &compilation,
  //  HotCache &hc,const Request *input)>,32> listeners;

  llvm::SmallVector<std::unique_ptr<const Intent>, 32> intentions;

public:
  Driver(const Driver &) = delete;
  void operator=(const Driver &) = delete;
  Driver() = delete;

public:
  Driver(llvm::StringRef programName, llvm::StringRef programPath);
  ~Driver();

  void Finish();

public:
  template <typename T, typename... Args> T *MakeIntent(Args &&...args) {
    auto result = new T(std::forward<Args>(args)...);
    intentions.emplace_back(result);
    return result;
  }

public:
  std::unique_ptr<llvm::opt::DerivedArgList>
  TranslateInputArgList(const llvm::opt::InputArgList &ial,
                        llvm::StringRef workDir);

  void ComputeLinkMode(const llvm::opt::InputArgList &ial);
  LinkMode GetLinkMode() const {
    return GetDriverOptions().outputOptions.linkMode;
  }

  bool JustLink() const {
    return (!GetDriverOptions().GetMode().CanCompile() &&
            (GetDriverOptions().outputOptions.linkMode != LinkMode::None));
  }

  bool CanLink() const {
    return (GetDriverOptions().GetMode().CanCompile() &&
            (GetDriverOptions().outputOptions.linkMode != LinkMode::None));
  }

  bool JustCompile() const {
    return (GetDriverOptions().GetMode().CanCompile() &&
            (GetDriverOptions().outputOptions.linkMode == LinkMode::None));
  }

  void ComputeOptions(const llvm::opt::InputArgList &ial);

  void ComputeOutputOptions(const ToolChain &toolChain,
                            const llvm::opt::InputArgList &ial,
                            const file::Files &inputs,
                            OutputOptions &outputOptions);

  CompilationModelKind
  ComputeCompilationModelKind(const llvm::opt::DerivedArgList &dal);

  std::unique_ptr<CompilationModel>
  ComputeCompilationModel(CompilationModelKind kind);

  CompilationModelKind GetCompilationModelKind() const {
    return GetDriverOptions().outputOptions.compilationModelKind;
  }

public:
  std::unique_ptr<ToolChain> BuildToolChain(const llvm::opt::InputArgList &ial);

  std::unique_ptr<Compilation> BuildCompilation(ToolChain &toolChain,
                                                llvm::opt::InputArgList &ial);

  std::unique_ptr<TaskQueue> BuildTaskQueue(const Compilation &compilation);

  void BuildJobRequests(Compilation &c, HotCache &hc, const file::Files &inputs,
                        const OutputOptions &outputOptions);

  void PrintJobRequests(HotCache &hc);

  void BuildJobs(Compilation &compilation, HotCache &hc,
                 const file::Files &inputs, const OutputOptions &outputOptions);

  // void PrintJobs(HotCache &hc);

public:
  file::Type GetInputFileType() const {
    return GetDriverOptions().inputFileType;
  }
  file::Type GetOutputFileType() const {
    return GetDriverOptions().outputOptions.outputFileType;
  }

  stone::Error ComputeOptions(llvm::opt::InputArgList &args) override;

  DriverOptions &GetDriverOptions() { return *driverOpts; }
  const DriverOptions &GetDriverOptions() const { return *driverOpts; }

  OutputFileMap &GetOutputFileMap() { return outputFileMap; }
  const OutputFileMap &GetOutputFileMap() const { return outputFileMap; }

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

  void PrintHelp(const llvm::opt::OptTable &opts);
  void PrintVersion();

public:
  void AddInputFile(llvm::StringRef name, unsigned fileID = 0);
  void AddInputFile(llvm::StringRef name, file::Type ty, unsigned fileID = 0);
  file::Files &BuildInputFiles(const llvm::opt::InputArgList &ial);
};

} // namespace stone

#endif
