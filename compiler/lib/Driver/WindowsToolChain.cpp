#include "stone/Driver/ToolChain.h"

using namespace stone;
using namespace stone::file;

WindowsToolChain::WindowsToolChain(const Driver &driver) : ToolChain(driver) {}

JobInvocation
WindowsToolChain::ConstructInvocation(const CompileJobConstruction &job,
                                      const JobContext &context) const {
  return JobInvocation();
}

JobInvocation
WindowsToolChain::ConstructInvocation(const DynamicLinkJobConstruction &job,
                                      const JobContext &context) const {

  return JobInvocation();
}

JobInvocation
WindowsToolChain::ConstructInvocation(const StaticLinkJobConstruction &job,
                                      const JobContext &context) const {
  return JobInvocation();
}

std::string WindowsToolChain::SanitizerRuntimeLibName(llvm::StringRef Sanitizer,
                                                      bool shared) const {}

void WindowsToolChain::AddPluginArguments(
    const llvm::opt::ArgList &args, llvm::opt::ArgStringList &arguments) const {

}

void WindowsToolChain::ValidateArguments(DiagnosticEngine &diags,
                                         const llvm::opt::ArgList &args,
                                         llvm::StringRef defaultTarget) const {}

void WindowsToolChain::ValidateOutputInfo(
    DiagnosticEngine &diags, const DriverOptions &driverOpts) const {}

std::string
WindowsToolChain::FindProgramRelativeToStoneImpl(StringRef name) const {
  return "TODO";
}

bool WindowsToolChain::ShouldStoreInvocationInDebugInfo() const { return true; }

std::string WindowsToolChain::GetGlobalDebugPathRemapping() const {
  return "TODO";
}

void WindowsToolChain::AddCommonCompileArgs(
    const DriverOptions &driverOpts, const CommandOutput &output,
    const llvm::opt::ArgList &inputArgs,
    llvm::opt::ArgStringList &arguments) const {}
