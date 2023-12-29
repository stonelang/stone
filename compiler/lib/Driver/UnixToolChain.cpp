#include "stone/Driver/ToolChain.h"

using namespace stone;

UnixToolChain::UnixToolChain(const Driver &driver) : ToolChain(driver) {}

std::string UnixToolChain::GetDefaultLinker() const { return "TODO"; }

LinuxToolChain::LinuxToolChain(const Driver &driver) : UnixToolChain(driver) {}

std::string LinuxToolChain::GetDefaultLinker() const { return "TODO"; }

JobInvocation
LinuxToolChain::ConstructInvocation(const CompileJobConstruction &job,
                                    const JobContext &context) const {
  return JobInvocation();
}

FreeBSDToolChain::FreeBSDToolChain(const Driver &driver)
    : UnixToolChain(driver) {}

OpenBSDToolChain::OpenBSDToolChain(const Driver &driver)
    : UnixToolChain(driver) {}

AndroidToolChain::AndroidToolChain(const Driver &driver)
    : UnixToolChain(driver) {}
