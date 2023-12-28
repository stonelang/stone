#include "stone/Driver/ToolChain.h"

using namespace stone;

LinuxToolChain::LinuxToolChain() : ToolChain(ToolChainKind::Linux) {}

JobInvocation
LinuxToolChain::ConstructInvocation(const CompileJobConstruction &job,
                                    const JobContext &context) const {

  return JobInvocation();
}
