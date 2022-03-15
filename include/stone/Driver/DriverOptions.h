#ifndef STONE_DRIVER_DRIVERNOPTIONS_H
#define STONE_DRIVER_DRIVERNOPTIONS_H

#include "stone/Option/Options.h"
#include "stone/Option/SessionOptions.h"

namespace stone {

enum class LTOKind { None, Full, Thin };

// CompileModel
enum class CompileModelKind {
  None,
  /// A single compile using a single compile invocation without -main-file.
  Single,
  /// Multiple compile invocations and -main-file.
  Multiple,
  /// A single batch that contains may 'Multiple' CompilingKind.
  Batch
};

enum class LinkKind {
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
class DriverOptions final {
  friend class Driver;

public:
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

  LinkKind linkKind = LinkKind::None;
  LTOKind ltoVariant = LTOKind::None;

  std::string libLTOPath;
  bool hasLibLTOPath;

  CompileModelKind compileModelKind = CompileModelKind::Multiple;

  /// The path to the SDK against which to build.
  /// (If empty, this implies no SDK.)
  std::string sdkPath;
  bool hasSdkPath = false;

  /// The name of the module which we are building.
  std::string moduleName;
  bool hasModuleName;

  /// The number of threads for multi-threaded compilation.
  unsigned numThreads = 0;

  /// Returns true if multi-threading is enabled.
  bool IsMultiThreading() const { return numThreads > 0; }

  bool RequiresLTO() { return ltoVariant != LTOKind::None; }
  bool RequiresLink() { return linkKind != LinkKind::None; }

  std::string stcPath;
  bool hasLangPath = false;
};
} // namespace stone

#endif
