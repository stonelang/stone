#include "stone/Driver/ToolChain.h"

using namespace stone;
using namespace stone::file;

WindowsToolChain::WindowsToolChain(const Driver &driver) : ToolChain(driver) {}

std::string WindowsToolChain::GetDefaultLinker() const { return ""; }
