
#include "stone/Driver/ToolChain.h"

using namespace stone;

WindowsToolChain::WindowsToolChain() : ToolChain(ToolChainKind::Windows) {}

JobInvocation
WindowsToolChain::ConstructInvocation(const CompileJobConstruction &job,
                                      const JobContext &context) const {
  return JobInvocation();
}