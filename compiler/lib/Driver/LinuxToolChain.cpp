#include "stone/Driver/ToolChain.h"

using namespace stone;
using namespace stone::file;

LinuxToolChain::LinuxToolChain(const Driver &driver) : UnixToolChain(driver) {}

std::string LinuxToolChain::GetDefaultLinker() const { return "TODO"; }

JobInvocation
LinuxToolChain::ConstructInvocation(const CompileJobConstruction &job,
                                    const JobContext &context) const {
  return JobInvocation();
}

JobInvocation
LinuxToolChain::ConstructInvocation(const DynamicLinkJobConstruction &job,
                                    const JobContext &context) const {

  return JobInvocation();
}

JobInvocation
LinuxToolChain::ConstructInvocation(const StaticLinkJobConstruction &job,
                                    const JobContext &context) const {
  return JobInvocation();
}

std::string LinuxToolChain::SanitizerRuntimeLibName(llvm::StringRef Sanitizer,
                                                    bool shared) const {}

void LinuxToolChain::AddPluginArguments(
    const llvm::opt::ArgList &args, llvm::opt::ArgStringList &arguments) const {

}

void LinuxToolChain::ValidateArguments(DiagnosticEngine &diags,
                                       const llvm::opt::ArgList &args,
                                       llvm::StringRef defaultTarget) const {}

void LinuxToolChain::ValidateOutputInfo(DiagnosticEngine &diags,
                                        const DriverOptions &driverOpts) const {

}

std::string
LinuxToolChain::FindProgramRelativeToStoneImpl(StringRef name) const {
  return "TODO";
}

bool LinuxToolChain::ShouldStoreInvocationInDebugInfo() const { return true; }

std::string LinuxToolChain::GetGlobalDebugPathRemapping() const {
  return "TODO";
}

void LinuxToolChain::AddCommonCompileArgs(
    const DriverOptions &driverOpts, const JobOutput &output,
    const llvm::opt::ArgList &inputArgs,
    llvm::opt::ArgStringList &arguments) const {}
