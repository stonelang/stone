#ifndef STONE_DRIVER_DRIVERNOPTIONS_H
#define STONE_DRIVER_DRIVERNOPTIONS_H

#include "stone/Basic/LangOptions.h"
#include "stone/Drive/OutputOptions.h"
#include "stone/Options/SessionOptions.h"

namespace stone {

// TODO: a lot of what is in DriverOutputProfile can go here
class DriverOptions final : public SessionOptions {
  friend class Driver;

public:
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
  DriverOptions(std::unique_ptr<Mode> mode) : SessionOptions(std::move(mode)) {}
};
} // namespace stone

#endif
