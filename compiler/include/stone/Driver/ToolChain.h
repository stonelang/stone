#ifndef STONE_DRIVER_DRIVER_TOOLCHAIN_H
#define STONE_DRIVER_DRIVER_TOOLCHAIN_H

#include "stone/Basic/STDAlias.h"
#include "stone/Diag/DiagnosticEngine.h"
#include "stone/Driver/DriverOptions.h"
#include "stone/Driver/Job.h"
#include "stone/Driver/JobConstruction.h"

#include "clang/Basic/DarwinSDKInfo.h"

#include "llvm/Option/ArgList.h"
#include "llvm/Support/Compiler.h"

namespace stone {

class Driver;
class JobInfo;
class Compilation;

/// Packs together information chosen by toolchains to create jobs.
class JobInvocation final {
public:
};

class ToolChain {
  const Driver &driver;
  mutable llvm::StringMap<std::string> programLookupCache;

protected:
  constexpr static const char *const StoneCompileExecutableName =
      "stone-compile";

protected:
  ToolChain(const Driver &driver);

public:
  virtual ~ToolChain();

public:
  const Driver &GetDriver() const { return driver; }
  ToolChainKind GetKind() const;

public:
  /// Handle arguments common to all invocations of the frontend (compilation,
  /// module-merging, LLDB's REPL, etc).
  virtual void AddCommonCompileArgs(const DriverOptions &driverOpts,
                                    const CommandOutput &output,
                                    const llvm::opt::ArgList &inputArgs,
                                    llvm::opt::ArgStringList &arguments) const;

  virtual void AddPlatformSpecificPluginCompileArgs(
      const DriverOptions &driverOpts, const CommandOutput &output,
      const llvm::opt::ArgList &inputArgs,
      llvm::opt::ArgStringList &arguments) const;

  /// Searches for the given executable in appropriate paths relative to the
  /// Swift binary.
  ///
  /// This method caches its results.
  ///
  /// \sa findProgramRelativeToSwiftImpl
  std::string FindProgramRelativeToStone(llvm::StringRef name) const;

  /// An override point for platform-specific subclasses to customize how to
  /// do relative searches for programs.
  ///
  /// This method is invoked by findProgramRelativeToSwift().
  virtual std::string
  FindProgramRelativeToStoneImpl(llvm::StringRef name) const;

  void AddInputsOfFileType(llvm::opt::ArgStringList &arguments,
                           llvm::ArrayRef<const JobConstruction *> inputs,
                           file::FileType inputFileType,
                           const char *prefixArgument = nullptr) const;

  void AddInputsOfFileType(llvm::opt::ArgStringList &arguments,
                           llvm::ArrayRef<const Job *> jobs,
                           const llvm::opt::ArgList &args,
                           file::FileType inputFileType,
                           const char *prefixArgument = nullptr) const;

  void AddPrimaryInputsOfFileType(llvm::opt::ArgStringList &arguments,
                                  llvm::ArrayRef<const Job *> jobs,
                                  const llvm::opt::ArgList &args,
                                  file::FileType inputFileType,
                                  const char *prefixArgument = nullptr) const;

  /// Get the resource dir link path, which is platform-specific and found
  /// relative to the compiler.
  void GetResourceDirPath(llvm::SmallVectorImpl<char> &runtimeLibPath,
                          const llvm::opt::ArgList &args, bool shared) const;

  /// Get the secondary runtime library link path given the primary path.
  void GetSecondaryResourceDirPath(
      llvm::SmallVectorImpl<char> &secondaryResourceDirPath,
      llvm::StringRef primaryPath) const;

  /// Get the runtime library link paths, which typically include the resource
  /// dir path and the SDK.
  void GetRuntimeLibraryPaths(SmallVectorImpl<std::string> &runtimeLibPaths,
                              const llvm::opt::ArgList &args,
                              llvm::StringRef SDKPath, bool shared) const;

  void AddPathEnvironmentVariableIfNeeded(
      Job::EnvironmentVector &env, const char *name, const char *separator,
      opts::OptID optionID, const llvm::opt::ArgList &args,
      ArrayRef<std::string> extraEntries = {}) const;

  /// Specific toolchains should override this to provide additional conditions
  /// under which the compiler invocation should be written into debug info. For
  /// example, Darwin does this if the RC_DEBUG_OPTIONS environment variable is
  /// set to match the behavior of Clang.
  virtual bool ShouldStoreInvocationInDebugInfo() const { return false; }

  /// Specific toolchains should override this to provide additional
  /// -debug-prefix-map entries. For example, Darwin has an RC_DEBUG_PREFIX_MAP
  /// environment variable that is also understood by Clang.
  virtual std::string GetGlobalDebugPathRemapping() const { return {}; }

public:
  /// Special handling for passing down '-l' arguments.
  ///
  /// Not all downstream tools (lldb, ld etc.) consistently accept
  /// a space between the '-l' flag and its argument, so we remove
  /// the extra space if it was present in \c Args.
  static void AddLinkedLibArgs(const llvm::opt::ArgList &args,
                               llvm::opt::ArgStringList &compileArgs);

  /// Returns whether a given sanitizer exists for the current toolchain.
  ///
  /// \param sanitizer Sanitizer name.
  /// \param shared Whether the library is shared
  bool SanitizerRuntimeLibExists(const llvm::opt::ArgList &args,
                                 StringRef sanitizer, bool shared = true) const;

public:
  /// Return the default language type to use for the given extension.
  /// If the extension is empty or is otherwise not recognized, return
  /// the invalid type \c TY_INVALID.
  file::FileType LookupFileTypeForExtension(llvm::StringRef ext) const;

  /// Copies the path for the directory clang libraries would be stored in on
  /// the current toolchain.
  void GetClangLibraryPath(const llvm::opt::ArgList &args,
                           llvm::SmallString<128> &libPath) const;

  // Returns the Clang driver executable to use for linking.
  const char *GetClangLinkerDriver(const llvm::opt::ArgList &args) const;

  /// Returns the name the clang library for a given sanitizer would have on
  /// the current toolchain.
  ///
  /// \param Sanitizer Sanitizer name.
  /// \param shared Whether the library is shared
  virtual std::string SanitizerRuntimeLibName(llvm::StringRef sanitizer,
                                              bool shared = true) const = 0;

  /// Adds a runtime library to the arguments list for linking.
  ///
  /// \param LibName The library name
  /// \param Arguments The arguments list to append to
  void AddLinkRuntimeLib(const llvm::opt::ArgList &Args,
                         llvm::opt::ArgStringList &Arguments,
                         StringRef LibName) const;

  virtual void AddPluginArguments(const llvm::opt::ArgList &Args,
                                  llvm::opt::ArgStringList &Arguments) const {}

  /// Validates arguments passed to the toolchain.
  ///
  /// An override point for platform-specific subclasses to customize the
  /// validations that should be performed.
  virtual void ValidateArguments(DiagnosticEngine &diags,
                                 const llvm::opt::ArgList &args,
                                 StringRef defaultTarget) const {}

  /// Validate the output information.
  ///
  /// An override point for platform-specific subclasses to customize their
  /// behavior once the outputs are known.
  virtual void ValidateOutputInfo(DiagnosticEngine &diags,
                                  const DriverOptions &driverOpts) const {}

  llvm::Expected<file::FileType>
  RemarkFileTypeFromArgs(const llvm::opt::ArgList &Args) const;

  /// If provided, and if the user has not already explicitly specified a
  /// linker to use via the "-fuse-ld=" option, this linker will be passed to
  /// the compiler invocation via "-fuse-ld=". Return an empty string to not
  /// specify any specific linker (the "-fuse-ld=" option will not be
  /// specified).
  ///
  /// The default behavior is to use the gold linker on ARM architectures,
  /// and to not provide a specific linker otherwise.
  virtual String GetDefaultLinker() const;

public:
  virtual JobInvocation ConstructInvocation(const CompileJobConstruction &job,
                                            const JobContext &context) const;

  virtual JobInvocation ConstructInvocation(const BackendJobConstruction &job,
                                            const JobContext &context) const;

  virtual JobInvocation
  ConstructInvocation(const StaticLinkJobConstruction &job,
                      const JobContext &context) const;

  virtual JobInvocation
  ConstructInvocation(const DynamicLinkJobConstruction &job,
                      const JobContext &context) const;

  virtual JobInvocation
  ConstructInvocation(const MergeModuleJobConstruction &job,
                      const JobContext &context) const;

  virtual JobInvocation
  ConstructInvocation(const InterpretJobConstruction &construction,
                      const JobContext &context) const;

  virtual JobInvocation
  ConstructInvocation(const AutolinkExtractJobConstruction &construction,
                      const JobContext &context) const;

  virtual JobInvocation
  ConstructInvocation(const GeneratePCHJobConstruction &construction,
                      const JobContext &context) const;

public:
  /// Construct a Job for the action \p JA, taking the given information into
  /// account.
  ///
  /// This method dispatches to the various \c ConstructInvocation methods,
  /// which may be overridden by platform-specific subclasses.
  Job *
  ConstructJob(const JobConstruction &jobConstruction, Compilation &compilation,
               llvm::SmallVectorImpl<const Job *> &&inputs,
               llvm::ArrayRef<const CompilationEntity *> inputConstructions,
               std::unique_ptr<CommandOutput> output) const;

  Job *ConstructJob(Compilation &compilation, const JobInfo *jobInfo) const;

  Job *ConstructJob(const JobConstruction &jobConstruction,
                    Compilation &compilation,
                    llvm::ArrayRef<const Job *> jobEntities,
                    llvm::ArrayRef<const CompilationEntity *> inputEntities,
                    std::unique_ptr<CommandOutput> output) const;
};

class DarwinToolChain final : public ToolChain {

public:
  DarwinToolChain(const Driver &driver);
  ~DarwinToolChain() = default;

protected:
  void AddLinkerInputArgs(JobInvocation &jobInvocation,
                          const JobContext &context) const;

  void AddSanitizerArgs(llvm::opt::ArgStringList &arguments,
                        const DynamicLinkJobConstruction &construction,
                        const JobContext &context) const;

  void AddArgsToLinkStdlib(llvm::opt::ArgStringList &Arguments,
                           const DynamicLinkJobConstruction &construction,
                           const JobContext &context) const;

  void AddProfileGenerationArgs(llvm::opt::ArgStringList &arguments,
                                const JobContext &context) const;

  void AddDeploymentTargetArgs(llvm::opt::ArgStringList &arguments,
                               const JobContext &context) const;

  void AddLTOLibArgs(llvm::opt::ArgStringList &arguments,
                     const JobContext &context) const;

public:
  /// < overrides
  JobInvocation ConstructInvocation(const DynamicLinkJobConstruction &job,
                                    const JobContext &context) const override;

  JobInvocation ConstructInvocation(const StaticLinkJobConstruction &job,
                                    const JobContext &context) const override;

  std::string SanitizerRuntimeLibName(llvm::StringRef Sanitizer,
                                      bool shared = true) const override;

  void AddPluginArguments(const llvm::opt::ArgList &args,
                          llvm::opt::ArgStringList &arguments) const override;

  void ValidateArguments(DiagnosticEngine &diags,
                         const llvm::opt::ArgList &args,
                         llvm::StringRef defaultTarget) const override;

  void ValidateOutputInfo(DiagnosticEngine &diags,
                          const DriverOptions &driverOpts) const override;

  std::string FindProgramRelativeToStoneImpl(StringRef name) const override;

  bool ShouldStoreInvocationInDebugInfo() const override;

  std::string GetGlobalDebugPathRemapping() const override;

  void AddCommonCompileArgs(const DriverOptions &driverOpts,
                            const CommandOutput &output,
                            const llvm::opt::ArgList &inputArgs,
                            llvm::opt::ArgStringList &arguments) const override;

public:
  /// Retrieve the target SDK version for the given target triple.
  llvm::Optional<llvm::VersionTuple>
  GetTargetSDKVersion(const llvm::Triple &triple) const;

  /// Information about the SDK that the application is being built against.
  /// This information is only used by the linker, so it is only populated
  /// when there will be a linker job.
  mutable llvm::Optional<clang::DarwinSDKInfo> darwinSDKInfo;

  const llvm::Optional<llvm::Triple> targetVariant;

public:
  static bool classof(const ToolChain *toolChain) {
    return toolChain->GetKind() == ToolChainKind::Darwin;
  }

public:
  DarwinToolChain *Create(const Driver &driver);
};

class UnixToolChain : public ToolChain {
protected:
  /// If provided, and if the user has not already explicitly specified a
  /// linker to use via the "-fuse-ld=" option, this linker will be passed to
  /// the compiler invocation via "-fuse-ld=". Return an empty string to not
  /// specify any specific linker (the "-fuse-ld=" option will not be
  /// specified).
  ///
  /// The default behavior is to use the gold linker on ARM architectures,
  /// and to not provide a specific linker otherwise.
  std::string GetDefaultLinker() const override;

  bool AddRuntimeRPath(const llvm::Triple &T,
                       const llvm::opt::ArgList &Args) const;

  JobInvocation
  ConstructInvocation(const InterpretJobConstruction &construction,
                      const JobContext &context) const override;

  JobInvocation
  ConstructInvocation(const AutolinkExtractJobConstruction &construction,
                      const JobContext &context) const override;

  // NOTE: Adding this
  JobInvocation ConstructInvocation(const CompileJobConstruction &construction,
                                    const JobContext &context) const override;

  JobInvocation
  ConstructInvocation(const DynamicLinkJobConstruction &construction,
                      const JobContext &context) const override;

  JobInvocation
  ConstructInvocation(const StaticLinkJobConstruction &construction,
                      const JobContext &context) const override;

public:
  UnixToolChain(const Driver &driver);
  ~UnixToolChain() = default;

public:
  std::string SanitizerRuntimeLibName(StringRef sanitizer,
                                      bool shared = true) const override;

  void AddPluginArguments(const llvm::opt::ArgList &args,
                          llvm::opt::ArgStringList &arguments) const override;

  void ValidateArguments(DiagnosticEngine &diags,
                         const llvm::opt::ArgList &args,
                         llvm::StringRef defaultTarget) const override;

  void ValidateOutputInfo(DiagnosticEngine &diags,
                          const DriverOptions &driverOpts) const override;

  std::string FindProgramRelativeToStoneImpl(StringRef name) const override;

  bool ShouldStoreInvocationInDebugInfo() const override;

  std::string GetGlobalDebugPathRemapping() const override;

  void AddCommonCompileArgs(const DriverOptions &driverOpts,
                            const CommandOutput &output,
                            const llvm::opt::ArgList &inputArgs,
                            llvm::opt::ArgStringList &arguments) const override;

public:
  static UnixToolChain *Create(const Driver &driver);
};

class LinuxToolChain final : public UnixToolChain {

public:
  LinuxToolChain(const Driver &driver);
  ~LinuxToolChain() = default;

public:
  JobInvocation ConstructInvocation(const CompileJobConstruction &job,
                                    const JobContext &context) const override;

  JobInvocation ConstructInvocation(const DynamicLinkJobConstruction &job,
                                    const JobContext &context) const override;

  JobInvocation ConstructInvocation(const StaticLinkJobConstruction &job,
                                    const JobContext &context) const override;

  std::string SanitizerRuntimeLibName(llvm::StringRef Sanitizer,
                                      bool shared = true) const override;

public:
  void AddPluginArguments(const llvm::opt::ArgList &args,
                          llvm::opt::ArgStringList &arguments) const override;

  void ValidateArguments(DiagnosticEngine &diags,
                         const llvm::opt::ArgList &args,
                         llvm::StringRef defaultTarget) const override;

  void ValidateOutputInfo(DiagnosticEngine &diags,
                          const DriverOptions &driverOpts) const override;

  std::string FindProgramRelativeToStoneImpl(StringRef name) const override;

  bool ShouldStoreInvocationInDebugInfo() const override;

  std::string GetGlobalDebugPathRemapping() const override;

  void AddCommonCompileArgs(const DriverOptions &driverOpts,
                            const CommandOutput &output,
                            const llvm::opt::ArgList &inputArgs,
                            llvm::opt::ArgStringList &arguments) const override;

public:
  static bool classof(const ToolChain *toolChain) {
    return toolChain->GetKind() == ToolChainKind::Linux;
  }

public:
  static LinuxToolChain *Create(const Driver &driver);
};

class WindowsToolChain final : public ToolChain {

protected:
  std::string GetDefaultLinker() const override;

public:
  WindowsToolChain(const Driver &driver);
  ~WindowsToolChain() = default;

public:
  JobInvocation ConstructInvocation(const CompileJobConstruction &job,
                                    const JobContext &context) const override;

  JobInvocation ConstructInvocation(const DynamicLinkJobConstruction &job,
                                    const JobContext &context) const override;

  JobInvocation ConstructInvocation(const StaticLinkJobConstruction &job,
                                    const JobContext &context) const override;

  std::string SanitizerRuntimeLibName(llvm::StringRef Sanitizer,
                                      bool shared = true) const override;

public:
  void AddPluginArguments(const llvm::opt::ArgList &args,
                          llvm::opt::ArgStringList &arguments) const override;

  void ValidateArguments(DiagnosticEngine &diags,
                         const llvm::opt::ArgList &args,
                         llvm::StringRef defaultTarget) const override;

  void ValidateOutputInfo(DiagnosticEngine &diags,
                          const DriverOptions &driverOpts) const override;

  std::string FindProgramRelativeToStoneImpl(StringRef name) const override;

  bool ShouldStoreInvocationInDebugInfo() const override;

  std::string GetGlobalDebugPathRemapping() const override;

  void AddCommonCompileArgs(const DriverOptions &driverOpts,
                            const CommandOutput &output,
                            const llvm::opt::ArgList &inputArgs,
                            llvm::opt::ArgStringList &arguments) const override;

public:
  static WindowsToolChain *Create(const Driver &driver);
};

} // namespace stone
#endif