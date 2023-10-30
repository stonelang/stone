#ifndef STONE_DRIVER_DRIVER_H
#define STONE_DRIVER_DRIVER_H

#include "stone/Basic/LLVM.h"
#include "stone/Basic/List.h"
#include "stone/Basic/OutputFileMap.h"
#include "stone/Basic/StatisticEngine.h"
#include "stone/Drive/BuildSystem.h"
#include "stone/Drive/CompilationListener.h"
#include "stone/Drive/CompilationModel.h"
#include "stone/Drive/DriverOptions.h"
#include "stone/Drive/JobAction.h"
#include "stone/Drive/ToolChain.h"
#include "stone/Session/Session.h"

namespace stone {

class Job;
class TaskQueue;

class Compilation;

class HotCache final {
  JobActionCache actionCache;
  JobCache jobCache;

public:
  JobActionCache &GetJobActionCache() { return actionCache; }
  JobCache &GetJobCache() { return jobCache; }
};

/// Expand response files in the argument list with retrying.
/// This function is a wrapper of lvm::cl::ExpandResponseFiles. It will
/// retry calling the function if the previous expansion failed.
void ExpandResponseFilesWithRetry(llvm::StringSaver &Saver,
                                  llvm::SmallVectorImpl<const char *> &Args);

/// Generates the list of arguments that would be passed to the compiler
/// invocation from the given driver arguments.
///
/// \param ArgList The driver arguments (i.e. normal arguments for \c stonec).
/// \param Diags The DiagnosticEngine used to report any errors parsing the
/// arguments.
/// \param JobAction Called with the list of invocation arguments if there were no
/// errors in processing \p ArgList. This is a callback rather than a return
/// value to avoid copying the arguments more than necessary.
/// \param ForceNoOutputs If true, override the output mode to "-typecheck" and
/// produce no outputs. For example, this disables "-emit-module" and "-c" and
/// prevents the creation of temporary files.
///
/// \returns True on error, or if \p JobAction returns true.
///
/// \note This function is not intended to create invocations which are
/// suitable for use in REPL or immediate modes.
bool GetSingleCompilerInvocationFromDriverArguments(
    ArrayRef<const char *> ArgList, DiagnosticEngine &Diags,
    llvm::function_ref<bool(ArrayRef<const char *> CompilerArgs)> JobAction,
    bool ForceNoOutputs = false);

} // end namespace stone

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

  llvm::SmallVector<std::unique_ptr<const JobAction>, 32> actionions;

public:
  Driver(const Driver &) = delete;
  void operator=(const Driver &) = delete;
  Driver() = delete;

public:
  Driver(llvm::StringRef programName, llvm::StringRef programPath);
  ~Driver();

  void Finish();

public:
  template <typename T, typename... Args> T *MakeJobAction(Args &&...args) {
    auto result = new T(std::forward<Args>(args)...);
    actionions.emplace_back(result);
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

  bool CanCompile() const { return GetDriverOptions().GetMode().CanCompile(); }

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
      // const JobJobAction *JA,
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
  // void AddInputFile(llvm::StringRef name, unsigned fileID = 0);
  // void AddInputFile(llvm::StringRef name, file::Type ty, unsigned fileID =
  // 0);

  file::Files &BuildInputFiles(const llvm::opt::InputArgList &ial);
};

} // namespace stone

#endif
