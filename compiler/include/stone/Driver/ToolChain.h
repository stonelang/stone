#ifndef STONE_DRIVER_DRIVER_EXECUTION_H
#define STONE_DRIVER_DRIVER_EXECUTION_H

#include "stone/Driver/Job.h"
#include "stone/Driver/JobConstruction.h"

namespace stone {

class Driver;
class Compilation;

/// Packs together information chosen by toolchains to create jobs.
class JobInvocation final {

  // public:
  //   const char *executableName;
  //   llvm::opt::ArgStringList arguments;
  //   std::vector<std::pair<const char *, const char *>> extraEnvironment;
  //   std::vector<FilelistInfo> FilelistInfos;

  //   // Not all platforms and jobs support the use of response files, so
  //   assume
  //   // "false" by default. If the executable specified in the InvocationInfo
  //   // constructor supports response files, this can be overridden and set to
  //   // "true".
  //   bool allowsResponseFiles = false;

  // public:

  //   JobInvocation(const char *name, llvm::opt::ArgStringList args = {},
  //                 decltype(extraEnvironment) extraEnv = {})
  //       : executableName(name), arguments(std::move(args)),
  //         extraEnvironment(std::move(extraEnv)) {}
};

enum class ToolChainKind { None = 0, Darwin, Linux, Windows };

class ToolChain {

  ToolChainKind kind;
  const Driver &driver;
  mutable llvm::StringMap<std::string> programLookupCache;

public:
  ToolChain(ToolChainKind kind, const Driver &driver);

public:
  ToolChainKind GetKind() { return kind; }
  const Driver &GetDriver() const { return driver; }

public:
  /// Handle arguments common to all invocations of the frontend (compilation,
  /// module-merging, LLDB's REPL, etc).
  virtual void AddCommonCompileArgs(const DriverOptions &driverOpts,
                                    const JobOutput &output,
                                    const llvm::opt::ArgList &inputArgs,
                                    llvm::opt::ArgStringList &arguments) const;

  virtual void AddPlatformSpecificPluginCompileArgs(
      const DriverOptions &driverOpts, const JobOutput &output,
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
                           file::Type inputFileType,
                           const char *prefixArgument = nullptr) const;

  void AddInputsOfFileType(llvm::opt::ArgStringList &arguments,
                           llvm::ArrayRef<const Job *> jobs,
                           const llvm::opt::ArgList &args,
                           file::Type inputFileType,
                           const char *prefixArgument = nullptr) const;

  void AddPrimaryInputsOfFileType(llvm::opt::ArgStringList &arguments,
                                  llvm::ArrayRef<const Job *> jobs,
                                  const llvm::opt::ArgList &args,
                                  file::Type inputFileType,
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

public:
  /// Construct a Job for the action \p JA, taking the given information into
  /// account.
  ///
  /// This method dispatches to the various \c ConstructInvocation methods,
  /// which may be overridden by platform-specific subclasses.
  std::unique_ptr<Job>
  ConstructJob(const JobConstruction &jobConstruction, Compilation &compilation,
               llvm::SmallVectorImpl<const Job *> &&inputs,
               llvm::ArrayRef<const JobConstruction *> inputConstructions,
               std::unique_ptr<JobOutput> output,
               const DriverOptions &driverOpts) const;
};

class DarwinToolChain final : public ToolChain {
public:
  DarwinToolChain(const Driver &driver);

public:
  JobInvocation ConstructInvocation(const CompileJobConstruction &job,
                                    const JobContext &context) const override;
};
class LinuxToolChain final : public ToolChain {
public:
  LinuxToolChain(const Driver &driver);

public:
  JobInvocation ConstructInvocation(const CompileJobConstruction &job,
                                    const JobContext &context) const override;
};

class WindowsToolChain final : public ToolChain {
public:
  WindowsToolChain(const Driver &driver);

public:
  JobInvocation ConstructInvocation(const CompileJobConstruction &job,
                                    const JobContext &context) const override;
};

} // namespace stone
#endif