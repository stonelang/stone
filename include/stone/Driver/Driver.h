#ifndef STONE_DRIVER_DRIVER_H
#define STONE_DRIVER_DRIVER_H

#include "stone/Core/LLVM.h"
#include "stone/Core/List.h"
#include "stone/Core/OutputFileMap.h"
#include "stone/Core/StatisticEngine.h"
#include "stone/Driver/BuildSystem.h"
#include "stone/Driver/DriverOptions.h"
#include "stone/Driver/ToolChain.h"

namespace stone {

class Compilation;
class BuildingIntents;
class BuildingJobs;

class CompilationOutputContext final {

public:
};

class Driver final {

  DriverOptions driverOpts;
  CompilationOutputContext coc;

  std::unique_ptr<ToolChain> toolChain;
  std::unique_ptr<BuildSystem> buildSystem;
  std::unique_ptr<Compilation> compilation;

  bool isLinkOnly = false;

  /// The OutputFileMap describing the Compilation's outputs, populated both by
  /// the user-provided output file map (if it exists) and inference rules that
  /// derive otherwise-unspecified output filenames from context.
  OutputFileMap outputFileMap;

public:
  Driver(const Driver &) = delete;
  void operator=(const Driver &) = delete;
  Driver() = delete;

public:
  Driver(CompilationListener *listener);
  Driver(const char *programName, const char *programPath,
         CompilationListener *listener);
  ~Driver();

  // virtual void Initialize() override;

  // Build the session
  void BuildSession(const llvm::opt::InputArgList &ial) override;

  virtual int Run() override;
  virtual void Finish() override;

private:
  struct Inflight;

public:
  void Build(llvm::ArrayRef<const char *> args) override;
  void BuildOutputContext();

  CompilationOutputContext &GetOuputContext() { return coc; }

  void BuildIntents(BuiltIntents &bi);
  void PrintIntents(BuiltIntents &bi);

  void BuildJobs(BuiltJobs &bj, BuiltIntents &bi);

  // void BuildOutputContext();

  void BuildToolChain(const llvm::opt::InputArgList &argList);

  void BuildCompilation();

public:
  file::Type GetInputFileType() const { return driverOpts.inputFileType; }
  file::Type GetOutputFileType() const { return driverOpts.outputFileType; }

  DriverOptions &GetDriverOptions() { return driverOpts; }
  const DriverOptions &GetDriverOptions() const { return driverOpts; }
  SessionOptions &GetOptions() override { return driverOpts; }

  OutputFileMap &GetOutputFileMap() { return outputFileMap; }
  const OutputFileMap &GetOutputFileMap() const { return outputFileMap; }

  bool IsCompileOnly() const { return driverOpts.linkKind == LinkKind::None; }
  bool IsCompilable() { return file::CanCompile(driverOpts.inputFileType); }

  void ComputeLinkKind();
  LinkKind GetLinkKind() const { return driverOpts.linkKind; }
  bool IsLinkable() const { return (GetLinkKind() != LinkKind::None); }
  bool IsLinkOnly() const { return isLinkOnly; }

  ToolChain &GetToolChain() { return *toolChain.get(); }

  CompileModelKind GetCompileModelKind() const {
    return driverOpts.compileModelKind;
  }

  Compilation &GetCompilation() { return *compilation.get(); }

  BuildSystem &GetBuild() { return *buildSystem.get(); }

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

  stone::CompileModelKind
  ComputeCompileModelKind(const llvm::opt::DerivedArgList &args,
                          const file::Files &inputs) const;

public:
  // IntentExecutor ConstructIntentExecutor(ProcessIntent& intent);
  // IntentExecutor ConstructIntentExecutor(CompilationIntent &intent);

protected:
  // ModeKind GetDefaultMode() override;
  // void BuildOptions() override;
  // llvm::StringRef GetSessionName() override;
  // llvm::StringRef GetSessionDesc() override;
};

} // namespace stone

#endif
