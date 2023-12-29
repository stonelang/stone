#include "stone/Driver/ToolChain.h"
#include "stone/Driver/Compilation.h"

using namespace stone;

ToolChain::ToolChain(ToolChainKind kind, const Driver &driver)
    : kind(kind), driver(driver) {}

JobInvocation ToolChain::ConstructInvocation(const CompileJobConstruction &job,
                                             const JobContext &context) const {

  return JobInvocation();
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
