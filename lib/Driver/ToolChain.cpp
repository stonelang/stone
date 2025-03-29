#include "stone/Driver/ToolChain.h"

using namespace stone;

ToolChain::ToolChain(const Driver &driver) : driver(driver) {}

DarwinToolChain::DarwinToolChain(const Driver &driver) : ToolChain(driver) {}