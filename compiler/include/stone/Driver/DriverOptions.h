#ifndef STONE_DRIVER_DRIVERNOPTIONS_H
#define STONE_DRIVER_DRIVERNOPTIONS_H

#include "stone/Basic/STDAlias.h"
#include "stone/Option/Action.h"
#include "stone/Option/Options.h"

#include "llvm/Option/Arg.h"
#include "llvm/Option/ArgList.h"

using namespace llvm::opt;

namespace stone {

enum class LinkMode : UInt8 {
  // We are not linking
  None = 0,
  // The default output compiling -- sc looks afor a main file and
  // outputs an executable file
  Executable,

  // The default library output: 'stone test.stone -emit-library -> test.dylib'
  DynamicLibrary,

  // The Library output that requires static: 'stone test.stone -emit-library
  // -satic -> test.a'
  StaticLibrary
};

/// This mode controls the compilation process
/// p := -primary-file
enum class CompilationKind : UInt8 {
  /// n input (s), n compile(s), n * n  parses
  /// Ex: compile_1(1=p ,...,n), compile_2(1,2=p,...,n),...,
  /// compile_n(1,....,n=p)
  Quadratic = 0,
  /// n input(s), n compile(s), n parses
  /// Ex: compile_1(1=p), compile_2(2=p),..., compile_n(n=p)
  Flat,
  /// n inputv(s), j CPU(s), j compile(s), n * j parses
  /// Ex: compile_1(1=p,...,n),...,
  /// compile_2(1,2=p,...,n),...,compile_j(1,...,p=j,...,n)
  CPUCount,
  /// n inputs, 1 compile, n parses
  /// Ex: compile(1,....,n)
  Single,
};

class DriverOptions final {
public:
  /// The main action requested or computed.
  Action mainAction;

public:
  /// Default target triple.
  std::string defaultTargetTriple;

  /// The path the executing program
  llvm::StringRef mainExecutablePath;

  /// The name of the executing program
  llvm::StringRef mainExecutableName;

public:
  /// The driver link mode
  LinkMode linkMode = LinkMode::None;

  /// The kind of compilation
  CompilationKind compilationKind = CompilationKind::Quadratic;

  /// The number of threads for multi-threaded compilation.
  unsigned numThreads = 0;

  /// Returns true if multi-threading is enabled.
  bool IsMultiThreading() const { return numThreads > 0; }

  /// The name of the module which we are building.
  std::string moduleName;

  /// The path to the SDK against which to build.
  /// (If empty, this implies no SDK.)
  std::string sdkPath;
};

} // namespace stone

#endif