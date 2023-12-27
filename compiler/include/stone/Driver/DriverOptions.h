#ifndef STONE_DRIVER_DRIVERNOPTIONS_H
#define STONE_DRIVER_DRIVERNOPTIONS_H

#include "stone/Basic/LangOptions.h"
#include "stone/Driver/OutputOptions.h"
#include "stone/Option/SessionOptions.h"

namespace stone {
class Driver;


// TODO: a lot of what is in DriverOutputProfile can go here
class DriverOptions final {
  friend DriverInstance;
  friend DriverInvocation;
  
public:
  /// The main action requested.
  Action mainAction;


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


  LinkMode linkMode = LinkMode::None;


  LTOKind ltoVariant = LTOKind::None;

  /// The output file type which should be used for the sc
  file::Type outputFileType = file::Type::None;

  std::string libLTOPath;

  CompilationModelKind compilationModelKind = CompilationModelKind::Quadratic;

  /// The number of threads for multi-threaded compilation.
  unsigned numThreads = 0;

  /// Whether or not the driver should generate a module.
  bool generateModule = false;

  /// Whether or not the driver should treat a generated module as a top-level
  /// output.
  bool treatModuleAsTopLevelOutput = false;

  /// Whether the compiler picked the current module name, rather than the user.
  bool moduleNameIsFallback = false;

public:
  bool HasLibLTOPath() const { return libLTOPath.size() > 0; }

  /// Returns true if multi-threading is enabled.
  bool IsMultiThreading() const { return numThreads > 0; }

  bool WithLTO() const { return ltoVariant != LTOKind::None; }
  bool CanLink() const { return linkMode != LinkMode::None; }


public:
  DriverOptions();
};
} // namespace stone

#endif
