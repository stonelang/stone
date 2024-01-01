#ifndef STONE_DRIVER_DRIVER_TOOLCHAIN_H
#define STONE_DRIVER_DRIVER_TOOLCHAIN_H

#include "stone/Diag/DiagnosticEngine.h"
#include "stone/Driver/DriverOptions.h"
#include "stone/Driver/Job.h"
#include "stone/Driver/JobConstruction.h"

namespace stone {

class Driver;
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
  virtual std::string GetDefaultLinker() const;

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

public:
  /// Construct a Job for the action \p JA, taking the given information into
  /// account.
  ///
  /// This method dispatches to the various \c ConstructInvocation methods,
  /// which may be overridden by platform-specific subclasses.
  Job *ConstructJob(const JobConstruction &jobConstruction,
                    Compilation &compilation,
                    llvm::SmallVectorImpl<const Job *> &&inputs,
                    llvm::ArrayRef<const JobConstruction *> inputConstructions,
                    std::unique_ptr<CommandOutput> output) const;
};

} // namespace stone
#endif