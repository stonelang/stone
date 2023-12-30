#include "stone/Driver/ToolChain.h"

using namespace stone;
using namespace stone::file;

WindowsToolChain::WindowsToolChain(const Driver &driver) : ToolChain(driver) {}

JobInvocation
WindowsToolChain::ConstructInvocation(const CompileJobConstruction &job,
                                      const JobContext &context) const {
  return JobInvocation();
}