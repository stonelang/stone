#ifndef STONE_DRIVER_OUTPUTOPTIONS_H
#define STONE_DRIVER_OUTPUTOPTIONS_H

#include "stone/Driver/Command.h"

namespace stone {

enum class LTOKind {
  None,
  ///
  Full,
  ///
  Thin
};
// CompilingModel
enum class CompilingModel : uint8_t {
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

class OutputOptions final {
public:
  LinkMode linkMode = LinkMode::None;
  LTOKind ltoVariant = LTOKind::None;

  /// The output file type which should be used for the sc
  file::Type outputFileType = file::Type::None;

  std::string libLTOPath;
  bool HasLibLTOPath() const { return libLTOPath.size() > 0; }

  CompilingModel compilingModel = CompilingModel::Multiple;

  /// The number of threads for multi-threaded compilation.
  unsigned numThreads = 0;

  /// Returns true if multi-threading is enabled.
  bool IsMultiThreading() const { return numThreads > 0; }

  bool RequiresLTO() const { return ltoVariant != LTOKind::None; }
  bool CanLink() const { return linkMode != LinkMode::None; }

  /// Whether or not the driver should generate a module.
  bool generateModule = false;

  /// Whether or not the driver should treat a generated module as a top-level
  /// output.
  bool treatModuleAsTopLevelOutput = false;

  /// Whether the compiler picked the current module name, rather than the user.
  bool moduleNameIsFallback = false;

  /// Might this sort of compile have explicit primary inputs?
  /// When running a single compile for the whole module (in other words
  /// "whole-module-optimization" mode) there must be no -primary-input's and
  /// nothing in a (preferably non-existent) -primary-filelist. Left to its own
  /// devices, the driver would forget to omit the primary input files, so
  /// return a flag here.
  bool MightHaveExplicitPrimaryInputs(const CommandOutput &output) const;

public:
  OutputOptions() {}
};

} // namespace stone

#endif
