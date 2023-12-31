#include "stone/Driver/ToolChain.h"

using namespace stone;
using namespace stone::file;

UnixToolChain::UnixToolChain(const Driver &driver) : ToolChain(driver) {}
std::string UnixToolChain::GetDefaultLinker() const { return "TODO"; }

// FreeBSDToolChain::FreeBSDToolChain(const Driver &driver)
//     : UnixToolChain(driver) {}

// OpenBSDToolChain::OpenBSDToolChain(const Driver &driver)
//     : UnixToolChain(driver) {}

// AndroidToolChain::AndroidToolChain(const Driver &driver)
//     : UnixToolChain(driver) {}
