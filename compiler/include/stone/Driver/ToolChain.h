#ifndef STONE_DRIVER_DRIVER_TOOLCHAIN_H
#define STONE_DRIVER_DRIVER_TOOLCHAIN_H

#include "stone/Diag/DiagnosticEngine.h"
#include "stone/Driver/Job.h"
#include "stone/Driver/JobConstruction.h"
#include "stone/Option/Options.h"

#include "llvm/Option/Option.h"

namespace stone {

class Driver;
class DriverInvocation;
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

class ToolChain {

  ToolChainKind kind;
  const Driver &driver;
  mutable llvm::StringMap<std::string> programLookupCache;

public:
  /// Special executable names.
  constexpr static const char *const StoneCompileExecutableName =
      "stone-compile";
  constexpr static const char *const StoneExecutableName = "stone";
  constexpr static const char *const LDExecutableName = "ld";
  constexpr static const char *const LLDExecutableName = "lld";
  constexpr static const char *const ClangPPExecutableName = "clang++";
  constexpr static const char *const ClangExecutableName = "clang";
  constexpr static const char *const GCCExecutableName = "g++";

public:
  ToolChain(ToolChainKind kind, const Driver &driver);
  virtual ~ToolChain() = default;

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
  file::Type LookupFileTypeForExtension(llvm::StringRef ext) const;

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

  llvm::Expected<file::Type>
  RemarkFileTypeFromArgs(const llvm::opt::ArgList &Args) const;

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
  Job *ConstructJob(const JobConstruction &jobConstruction,
                    Compilation &compilation,
                    llvm::SmallVectorImpl<const Job *> &&inputs,
                    llvm::ArrayRef<const JobConstruction *> inputConstructions,
                    std::unique_ptr<JobOutput> output,
                    const DriverInvocation &invocation) const;
};

class DarwinToolChain final : public ToolChain {
public:
  DarwinToolChain(const Driver &driver);

public:
  JobInvocation ConstructInvocation(const DynamicLinkJobConstruction &job,
                                    const JobContext &context) const override;

  JobInvocation ConstructInvocation(const StaticLinkJobConstruction &job,
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

// class UnixToolChain : public ToolChain {
//   // protected:
//   //   InvocationInfo constructInvocation(const InterpretJobAction &job,
//   //                                      const JobContext &context) const
//   //                                      override;
//   //   InvocationInfo constructInvocation(const AutolinkExtractJobAction
//   &job,
//   //                                      const JobContext &context) const
//   //                                      override;

// protected:
//   /// If provided, and if the user has not already explicitly specified a
//   /// linker to use via the "-fuse-ld=" option, this linker will be passed to
//   /// the compiler invocation via "-fuse-ld=". Return an empty string to not
//   /// specify any specific linker (the "-fuse-ld=" option will not be
//   /// specified).
//   ///
//   /// The default behavior is to use the gold linker on ARM architectures,
//   /// and to not provide a specific linker otherwise.
//   virtual std::string GetDefaultLinker() const;
// };

// class FreeBSDToolChain : public UnixToolChain {

// };

// class OpenBSDToolChain : public UnixToolChain {

// };

} // namespace stone
#endif