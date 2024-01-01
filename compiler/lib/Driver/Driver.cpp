#include "stone/Driver/Driver.h"
#include "stone/Diag/CoreDiagnostic.h"
#include "stone/Diag/DriverDiagnostic.h"
#include "stone/Driver/DriverAllocation.h"
#include "stone/Driver/Job.h"
#include "stone/Strings.h"

#include "llvm/Support/Host.h"
#include "llvm/Support/Path.h"

using namespace stone;

using namespace llvm::opt;

Driver::Driver() {}

Driver::~Driver() {}

void *stone::AllocateInDriver(size_t bytes, const stone::Driver &driver,
                              unsigned alignment) {
  return driver.Allocate(bytes, alignment);
}
