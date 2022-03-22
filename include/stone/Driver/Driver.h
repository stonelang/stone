#ifndef STONE_DRIVER_DRIVER_H
#define STONE_DRIVER_DRIVER_H

#include "stone/Core/LLVM.h"
#include "stone/Core/List.h"
#include "stone/Core/OutputFileMap.h"
#include "stone/Core/StatisticEngine.h"
#include "stone/Driver/BuildSystem.h"
#include "stone/Driver/CompilationListener.h"
#include "stone/Driver/DriverOptions.h"
#include "stone/Driver/Request.h"
#include "stone/Driver/ToolChain.h"
#include "stone/Session/Session.h"

namespace stone {

class Job;
class JobRequest;
class Compilation;

class ReqCache final {
public:
  Request *currentRequest;
  /// We keep track of the inputs for the module that we are building.
  /// These are CompileJobRequest
  llvm::SmallVector<const Request *, 4> forModule;

  /// When are building the  request(s), keep track of the linker dependecies
  llvm::SmallVector<const Request *, 2> forLink;

  /// These are the top-level job requests -- we use them recursively to build
  /// out the "real" jobs.
  llvm::SmallVector<const Request *, 16> forTop;

  bool ForModule() { return forModule.size(); }
  void CacheForModule(const Request *request) { forModule.push_back(request); }

  bool ForLink() { return forLink.size(); }
  void CacheForLink(const Request *request) { forLink.push_back(request); }

  bool ForTop() { return forTop.size(); }
  void CacheForTop(const Request *request) { forTop.push_back(request); }

public:
  void Finish(Compilation &compilation, const OutputOptions &outputOpts);
};

class HotCache final {
  ReqCache reqCache;
  JobCache jobCache;

public:
  ReqCache &GetReqCache() { return reqCache; }
  JobCache &GetJobCache() { return jobCache; }
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
  // llvm::SmallVector<std::function<void(Compilation &compilation,
  //  HotCache &hc,const Request *input)>,32> listeners;

  llvm::SmallVector<std::unique_ptr<const Request>, 32> requests;

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
  template <typename T, typename... Args> T *MakeRequest(Args &&...args) {
    auto result = new T(std::forward<Args>(args)...);
    requests.emplace_back(result);
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

  bool JustLink() const {
    return (!GetMode().CanCompile() &&
            (driverOpts.outputOptions.linkMode != LinkMode::None));
  }

  bool CanLink() const {
    return (GetMode().CanCompile() &&
            (driverOpts.outputOptions.linkMode != LinkMode::None));
  }

  bool JustCompile() const {
    return (GetMode().CanCompile() &&
            (driverOpts.outputOptions.linkMode == LinkMode::None));
  }

  void ComputeOptions(const llvm::opt::InputArgList &ial);

  void ComputeOutputOptions(const ToolChain &toolChain,
                            const llvm::opt::InputArgList &ial,
                            const file::Files &inputs,
                            OutputOptions &outputOptions);

  CompilationMode ComputeCompilationMode(const llvm::opt::DerivedArgList &dal,
                                         bool &isBatchModel) const;

  CompilationMode GetCompilationMode() const {
    return driverOpts.outputOptions.compilationMode;
  }

public:
  std::unique_ptr<ToolChain> BuildToolChain(const llvm::opt::InputArgList &ial);
  std::unique_ptr<Compilation> BuildCompilation(ToolChain &toolChain,
                                                llvm::opt::InputArgList &ial);

  void BuildJobRequests(Compilation &c, HotCache &hc, const file::Files &inputs,
                        const OutputOptions &outputOptions);

  void PrintJobRequests(HotCache &hc);

  void BuildJobs(Compilation &compilation, HotCache &hc,
                 const file::Files &inputs, const OutputOptions &outputOptions);

  // void PrintJobs(HotCache &hc);

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
