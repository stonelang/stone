#include "stone/Driver/ToolChain.h"

using namespace stone;

LinuxToolChain::LinuxToolChain(const Driver &driver)
    : ToolChain(ToolChainKind::Linux, driver) {}

JobInvocation
LinuxToolChain::ConstructInvocation(const CompileJobConstruction &job,
                                    const JobContext &context) const {

  return JobInvocation();
}
