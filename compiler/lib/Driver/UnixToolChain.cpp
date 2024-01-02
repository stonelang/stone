#include "stone/Driver/ToolChain.h"

using namespace stone;
using namespace stone::file;

UnixToolChain::UnixToolChain(const Driver &driver) : ToolChain(driver) {}

std::string UnixToolChain::GetDefaultLinker() const {
  // if (driverOpts.GetTriple().isAndroid()){
  //   return "lld";
  // }
  // switch (driver.GetDriverOptions().GetTriple().getArch()) {
  // case llvm::Triple::arm:
  // case llvm::Triple::aarch64:
  // case llvm::Triple::aarch64_32:
  // case llvm::Triple::armeb:
  // case llvm::Triple::thumb:
  // case llvm::Triple::thumbeb:
  //   // BFD linker has issues wrt relocation of the protocol conformance
  //   // section on these targets, it also generates COPY relocations for
  //   // final executables, as such, unless specified, we default to gold
  //   // linker.
  //   return "gold";
  // case llvm::Triple::x86:
  // case llvm::Triple::x86_64:
  // case llvm::Triple::ppc64:
  // case llvm::Triple::ppc64le:
  // case llvm::Triple::systemz:
  //   // BFD linker has issues wrt relocations against protected symbols.
  //   return "gold";
  // default:
  //   // Otherwise, use the default BFD linker.
  //   return "";
  // }
  return "";
}

JobInvocation
UnixToolChain::ConstructInvocation(const CompileJobConstruction &job,
                                   const JobContext &context) const {
  return JobInvocation();
}

JobInvocation
UnixToolChain::ConstructInvocation(const DynamicLinkJobConstruction &job,
                                   const JobContext &context) const {

  return JobInvocation();
}

JobInvocation
UnixToolChain::ConstructInvocation(const StaticLinkJobConstruction &job,
                                   const JobContext &context) const {
  return JobInvocation();
}

JobInvocation
UnixToolChain::ConstructInvocation(const InterpretJobConstruction &job,
                                   const JobContext &context) const {
  return JobInvocation();
}

JobInvocation UnixToolChain::ConstructInvocation(
    const AutolinkExtractJobConstruction &construction,
    const JobContext &context) const {}

std::string
UnixToolChain::FindProgramRelativeToStoneImpl(StringRef name) const {

  return "TODO";
}
std::string UnixToolChain::SanitizerRuntimeLibName(llvm::StringRef Sanitizer,
                                                   bool shared) const {}

void UnixToolChain::AddPluginArguments(
    const llvm::opt::ArgList &args, llvm::opt::ArgStringList &arguments) const {

}

void UnixToolChain::ValidateArguments(DiagnosticEngine &diags,
                                      const llvm::opt::ArgList &args,
                                      llvm::StringRef defaultTarget) const {}

void UnixToolChain::ValidateOutputInfo(DiagnosticEngine &diags,
                                       const DriverOptions &driverOpts) const {}

// std::string
// UnixToolChain::FindProgramRelativeToStoneImpl(StringRef name) const {
//   return "TODO";
// }

bool UnixToolChain::ShouldStoreInvocationInDebugInfo() const { return true; }

std::string UnixToolChain::GetGlobalDebugPathRemapping() const {
  return "TODO";
}

void UnixToolChain::AddCommonCompileArgs(
    const DriverOptions &driverOpts, const CommandOutput &output,
    const llvm::opt::ArgList &inputArgs,
    llvm::opt::ArgStringList &arguments) const {}
