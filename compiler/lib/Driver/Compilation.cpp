#include "stone/Driver/Compilation.h"
#include "stone/Basic/Defer.h"
#include "stone/Driver/Driver.h"
#include "stone/Driver/Task.h"

using namespace stone;

Compilation::Compilation(const Driver &driver) : driver(driver) {}