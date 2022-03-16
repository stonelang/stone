#ifndef STONE_DRIVER_DRIVERNOPTIONS_H
#define STONE_DRIVER_DRIVERNOPTIONS_H

#include "stone/Core/SystemOptions.h"
#include "stone/Option/BaseOptions.h"

namespace stone {

enum class LTOKind { None, Full, Thin };

// CompileModel
enum class CompileModel : uint8_t {
  None,
  /// There is no linking in this mode -- we pass all the files to the compile
  /// command This scenario will not have a file with 'fun Main()'
  Single,
  /// There is linking in this mode. So, we pass each file to the compile
  /// command which produces an object file
  Multiple,
  /// A single batch that contains may 'Multiple' CompilingKind.
  Batch
};

enum class LinkMode : uint8_t {
  None,
  // The default output compiling -- sc looks afor a main file and
  // outputs an executable file
  EmitExecutable,

  // The default library output: 'stone test.stone -emit-library -> test.dylib'
  EmitDynamicLibrary,

  // The Library output that requires static: 'stone test.stone -emit-library
  // -satic -> test.a'
  EmitStaticLibrary
};

// TODO: a lot of what is in DriverOutputProfile can go here
class DriverOptions final : public opts::BaseOptions {
  friend class Driver;

public:
  SystemOptions systemOpts;

  /// Print the jobs in the JobQueue
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
  bool printIntents = false;

  // Whether or not the compilation should generate a module.
  bool generateModule = false;

  // True if temporary files should not be deleted.
  bool saveTempFiles = false;

  LinkMode linkMode = LinkMode::None;
  LTOKind ltoVariant = LTOKind::None;

  std::string libLTOPath;
  bool HasLibLTOPath() const { return libLTOPath.size() > 0; }

  CompileModel compileModel = CompileModel::Multiple;

  /// The number of threads for multi-threaded compilation.
  unsigned numThreads = 0;

  /// Returns true if multi-threading is enabled.
  bool IsMultiThreading() const { return numThreads > 0; }

  bool RequiresLTO() { return ltoVariant != LTOKind::None; }
  bool RequiresLink() { return linkMode != LinkMode::None; }

  std::string scPath;
  bool HasSCPath() const { return scPath.size() > 0; }

public:
  DriverOptions() { defaultModeKind = ModeKind::None; }
};
} // namespace stone

#endif
