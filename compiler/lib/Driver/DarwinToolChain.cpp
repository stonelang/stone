#include "stone/Driver/ToolChain.h"

using namespace stone;
using namespace stone::file;

DarwinToolChain::DarwinToolChain(const Driver &driver) : ToolChain(driver) {}

JobInvocation
DarwinToolChain::ConstructInvocation(const DynamicLinkJobConstruction &job,
                                     const JobContext &context) const {

  return JobInvocation();
}

JobInvocation
DarwinToolChain::ConstructInvocation(const StaticLinkJobConstruction &job,
                                     const JobContext &context) const {

  return JobInvocation();
}

std::string DarwinToolChain::SanitizerRuntimeLibName(llvm::StringRef Sanitizer,
                                                     bool shared) const {}

void DarwinToolChain::AddPluginArguments(
    const llvm::opt::ArgList &args, llvm::opt::ArgStringList &arguments) const {

}

void DarwinToolChain::ValidateArguments(DiagnosticEngine &diags,
                                        const llvm::opt::ArgList &args,
                                        llvm::StringRef defaultTarget) const {}

void DarwinToolChain::ValidateOutputInfo(
    DiagnosticEngine &diags, const DriverOptions &driverOpts) const {}

std::string
DarwinToolChain::FindProgramRelativeToStoneImpl(llvm::StringRef name) const {
  return "";
}

bool DarwinToolChain::ShouldStoreInvocationInDebugInfo() const { return false; }

std::string DarwinToolChain::GetGlobalDebugPathRemapping() const { return ""; }

void DarwinToolChain::AddCommonCompileArgs(
    const DriverOptions &driverOpts, const CommandOutput &output,
    const llvm::opt::ArgList &inputArgs,
    llvm::opt::ArgStringList &arguments) const {}
