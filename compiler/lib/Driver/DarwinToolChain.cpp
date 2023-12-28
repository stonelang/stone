#include "stone/Driver/ToolChain.h"

using namespace stone;

DarwinToolChain::DarwinToolChain() : ToolChain(ToolChainKind::Darwin) {}

JobInvocation
DarwinToolChain::ConstructInvocation(const CompileJobConstruction &job,
                                     const JobContext &context) const {

  return JobInvocation();
}
