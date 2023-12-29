#include "stone/Driver/ToolChain.h"

using namespace stone;

DarwinToolChain::DarwinToolChain(const Driver &driver)
    : ToolChain(ToolChainKind::Darwin, driver) {}

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
