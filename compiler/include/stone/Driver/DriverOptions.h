#ifndef STONE_DRIVER_DRIVERNOPTIONS_H
#define STONE_DRIVER_DRIVERNOPTIONS_H

#include "stone/Basic/LangOptions.h"
#include "stone/Driver/OutputOptions.h"
#include "stone/Option/SessionOptions.h"

namespace stone {
class Driver;
class DriverAction final : public Action {
  friend Driver;

public:
  DriverAction(const DriverAction &) = delete;
  void operator=(const DriverAction &) = delete;
  DriverAction(DriverAction &&) = delete;
  void operator=(DriverAction &&) = delete;

public:
  DriverAction();
};
// TODO: a lot of what is in DriverOutputProfile can go here
class DriverOptions final {
  friend Driver;

public:
  /// The input action entered. e.g., --parse ...
  DriverAction action;

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

  /// Print the driver actionions
  bool printRequests = false;

  // True if temporary files should not be deleted.
  bool saveTempFiles = false;

  std::string scPath;
  bool HasSCPath() const { return scPath.size() > 0; }

  OutputOptions outputOptions;

  /// a SmallVector of ctx files
  file::Files inputFiles;

  /// The file input type
  file::Type inputFileType = file::Type::None;

  /// The output file type which should be used for the sc
  file::Type outputFileType = file::Type::None;

public:
  DriverOptions();
};
} // namespace stone

#endif
