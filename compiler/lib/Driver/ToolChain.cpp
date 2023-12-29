#include "stone/Driver/ToolChain.h"
#include "stone/Driver/Compilation.h"

using namespace stone;

ToolChain::ToolChain(const Driver &driver) : driver(driver) {}

ToolChain::~ToolChain() {}

ToolChainKind ToolChain::GetKind() const {
  return driver.GetInvocation().GetToolChainKind();
}

JobInvocation
ToolChain::ConstructInvocation(const CompileJobConstruction &construction,
                               const JobContext &context) const {

  return JobInvocation();
}

JobInvocation ToolChain::ConstructInvocation(const BackendJobConstruction &job,
                                             const JobContext &context) const {
  return JobInvocation();
}

JobInvocation
ToolChain::ConstructInvocation(const MergeModuleJobConstruction &job,
                               const JobContext &context) const {

  return JobInvocation();
  ;
}

JobInvocation
ToolChain::ConstructInvocation(const DynamicLinkJobConstruction &construction,
                               const JobContext &context) const {
  llvm_unreachable("linking not implemented for this toolchain");
}

JobInvocation
ToolChain::ConstructInvocation(const StaticLinkJobConstruction &construction,
                               const JobContext &context) const {
  llvm_unreachable("archiving not implemented for this toolchain");
}

void ToolChain::AddPlatformSpecificPluginCompileArgs(
    const DriverOptions &driverOpts, const JobOutput &output,
    const llvm::opt::ArgList &inputArgs,
    llvm::opt::ArgStringList &arguments) const {}

void ToolChain::AddCommonCompileArgs(
    const DriverOptions &driverOpts, const JobOutput &output,
    const llvm::opt::ArgList &inputArgs,
    llvm::opt::ArgStringList &arguments) const {}

std::string
ToolChain::FindProgramRelativeToStoneImpl(llvm::StringRef name) const {
  return "";
}

// Returns the Clang driver executable to use for linking.
const char *
ToolChain::GetClangLinkerDriver(const llvm::opt::ArgList &args) const {

  // NOTE: using clang for now -- may consider clang++ in the future.
  const char *clangLinkerDriver = "clang";

  if (const Arg *arg = args.getLastArg(opts::ToolsDirectory)) {
    llvm::StringRef toolChainPath(arg->getValue());

    // If there is a linker driver in the toolchain folder, use that instead.
    if (auto tool =
            llvm::sys::findProgramByName(clangLinkerDriver, {toolChainPath}))
      clangLinkerDriver = args.MakeArgString(tool.get());
  }
  return clangLinkerDriver;
}
