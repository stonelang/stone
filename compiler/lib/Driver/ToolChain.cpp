#include "stone/Driver/ToolChain.h"
#include "stone/Driver/Compilation.h"

using namespace stone;
using namespace stone::file;



ToolChain::ToolChain(const Driver &driver) : driver(driver) {}

ToolChain::~ToolChain() {}

ToolChainKind ToolChain::GetKind() const {
  return driver.GetDriverOptions().GetToolChainKind();
}