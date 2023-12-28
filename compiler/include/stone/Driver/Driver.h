#ifndef STONE_DRIVER_DRIVER_H
#define STONE_DRIVER_DRIVER_H

#include "stone/Driver/DriverOptions.h"

using namespace llvm::opt;

namespace stone {

class Driver final {

  DriverOptions driverOpts;
  std::unique_ptr<llvm::opt::OptTable> optTable;

public:
  Driver(const Driver &) = delete;
  void operator=(const Driver &) = delete;
  Driver(Driver &&) = delete;
  void operator=(Driver &&) = delete;

public:
  Driver();

public:
  std::unique_ptr<InputArgList>
  ParseCommandLine(llvm::ArrayRef<const char *> args);

public:
  DriverOptions &GetDriverOptions() { return driverOpts; }
  const DriverOptions &GetDriverOptions() const { return driverOpts; }
};

} // namespace stone
#endif