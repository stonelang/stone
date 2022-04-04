#ifndef STONE_DRIVER_DRIVERNOPTIONS_H
#define STONE_DRIVER_DRIVERNOPTIONS_H

#include "stone/Basic/SystemOptions.h"
#include "stone/Driver/OutputOptions.h"
#include "stone/Session/BaseOptions.h"

namespace stone {

// TODO: a lot of what is in DriverOutputProfile can go here
class DriverOptions final : public BaseOptions {
  friend class Driver;

public:
  // TODO: Move to base
  SystemOptions systemOpts;

  /// Print the jobs in the TaskQueue
  bool printJobs = false;

  /// Use the default LD linker
  bool useLDLinker = true;

  /// Override the default LDLinker
  bool useLLDLinker = false;

  /// Show the entire lifcycle of the compilation
  bool printLifecycle = false;

  /// Clean the build before compilation
  bool cleanBuild = false;

  /// Print the driver intentions
  bool printRequests = false;

  // True if temporary files should not be deleted.
  bool saveTempFiles = false;

  std::string scPath;
  bool HasSCPath() const { return scPath.size() > 0; }

  OutputOptions outputOptions;

public:
  DriverOptions() { defaultModeKind = ModeKind::None; }
};
} // namespace stone

#endif
