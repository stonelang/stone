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
/// This mode controls the compilation process
/// p := -primary-file
enum class CompilationMode : unsigned {
  /// n inputs, n compile(s), n * n  parses
  /// Ex: compile_1(1=p ,...,n), compile_2(1,2=p,...,n),...,
  /// compile_n(1,....,n=p)
  Quadratic = 0,
  /// n input(s), n compile(s), n parses
  /// Ex: compile_1(1=p), compile_2(2=p),..., compile_n(n=p)
  Flat,
  /// n inputs, j CPU(s), j compile(s), n * j parses
  /// Ex: compile_1(1=p,...,n),...,
  /// compile_2(1,2=p,...,n),...,compile_j(1,...,p=j,...,n)
  CPU,
  /// n inputs, 1 compile, n parses
  /// Ex: compile(1,....,n)
  Single,
};

enum class LinkMode : uint8_t {
  // We are not linking
  None = 0,
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

  CompilationMode compilationMode = CompilationMode::Quadratic;

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
