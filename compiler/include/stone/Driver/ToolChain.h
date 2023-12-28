#ifndef STONE_DRIVER_DRIVER_EXECUTION_H
#define STONE_DRIVER_DRIVER_EXECUTION_H

#include "stone/Driver/Job.h"
#include "stone/Driver/JobConstruction.h"

namespace stone {

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

public:
  ToolChain(ToolChainKind kind);

public:
  ToolChainKind GetKind() { return kind; }

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
  DarwinToolChain();

public:
  JobInvocation ConstructInvocation(const CompileJobConstruction &job,
                                    const JobContext &context) const override;
};
class LinuxToolChain final : public ToolChain {
public:
  LinuxToolChain();

public:
  JobInvocation ConstructInvocation(const CompileJobConstruction &job,
                                    const JobContext &context) const override;
};

class WindowsToolChain final : public ToolChain {
public:
  WindowsToolChain();

public:
  JobInvocation ConstructInvocation(const CompileJobConstruction &job,
                                    const JobContext &context) const override;
};

} // namespace stone
#endif