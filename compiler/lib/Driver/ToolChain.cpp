#include "stone/Driver/ToolChain.h"
#include "stone/Driver/Compilation.h"
#include "stone/Option/Options.h"

using namespace stone;
using namespace stone::file;

ToolChain::ToolChain(const Driver &driver) : driver(driver) {}

ToolChain::~ToolChain() {}

ToolChainKind ToolChain::GetKind() const {
  return driver.GetDriverOptions().GetToolChainKind();
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

JobInvocation
ToolChain::ConstructInvocation(const AutolinkExtractJobConstruction &construction,
                               const JobContext &context) const {
  llvm_unreachable("autolink extraction not implemented for this toolchain");
}

JobInvocation
ToolChain::ConstructInvocation(const InterpretJobConstruction &job,
                                   const JobContext &context) const {
  return JobInvocation();
}




void ToolChain::AddPlatformSpecificPluginCompileArgs(
    const DriverOptions &driverOpts, const CommandOutput &output,
    const llvm::opt::ArgList &inputArgs,
    llvm::opt::ArgStringList &arguments) const {}

void ToolChain::AddCommonCompileArgs(
    const DriverOptions &driverOpts, const CommandOutput &output,
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
  const char *clangLinkerDriver = GetDefaultLinker().c_str();

  if (const llvm::opt::Arg *arg = args.getLastArg(opts::ToolsDirectory)) {
    llvm::StringRef toolChainPath(arg->getValue());

    // If there is a linker driver in the toolchain folder, use that instead.
    if (auto tool =
            llvm::sys::findProgramByName(clangLinkerDriver, {toolChainPath})) {
      clangLinkerDriver = args.MakeArgString(tool.get());
    }
  }
  return clangLinkerDriver;
}

String ToolChain::GetDefaultLinker() const { return "clang"; }
