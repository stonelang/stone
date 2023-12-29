#include "stone/Driver/ToolChain.h"

using namespace stone;

WindowsToolChain::WindowsToolChain(const Driver &driver) : ToolChain(driver) {}

JobInvocation
WindowsToolChain::ConstructInvocation(const CompileJobConstruction &job,
                                      const JobContext &context) const {
  return JobInvocation();
}