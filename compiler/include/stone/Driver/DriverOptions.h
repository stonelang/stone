#ifndef STONE_DRIVER_DRIVER_OPTIONS_H
#define STONE_DRIVER_DRIVER_OPTIONS_H

#include "stone/Basic/CodeGenOptions.h"
#include "stone/Basic/LangOptions.h"
#include "stone/Basic/STDAlias.h"
#include "stone/Basic/Status.h"
#include "stone/Driver/CompilationEntity.h"
#include "stone/Option/Options.h"

#include "llvm/ADT/SetVector.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/Triple.h"

namespace stone {

class CompilationInputFile;
class DriverInputsConverter;

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


/// An enum providing different levels of output which should be produced
/// by a Compilation.
enum class CompilationOutputLevel {
  /// Indicates that normal output should be produced.
  Normal,

  /// Indicates that only jobs should be printed and not run. (-###)
  PrintJobs,

  /// Indicates that verbose output should be produced. (-v)
  Verbose,

  /// Indicates that parseable output should be produced.
  Parseable,
};

/// This mode controls the manner in with compile is invoked.
/// p := -primary-file
enum class CompileStyle : UInt8 {
  /// n input (s), n compile(s), n * n  parses
  /// Ex: compile_1(1=p ,...,n), compile_2(1,2=p,...,n),...,
  /// compile_n(1,....,n=p)
  Normal = 0,
  /// n inputs, 1 compile, n parses, p := 0
  /// Ex: compile(1,....,n)
  Single,
  /// n input(s), n compile(s), n parses
  /// Ex: compile_1(1=p), compile_2(2=p),..., compile_n(n=p)
  Flat,
  /// n input (s), j CPU(s), j p(s), j compile(s), n * j parses
  /// Ex: compile_1(1=p,...,n), compile_2(1,2=p,...,n),...,
  /// compile_j(1,...,p=j,...,n)
  CPUCount,

};

/// Only for CompilationInputFiles
class DriverInputsAndOutputs final {

  friend DriverInputsConverter;
  std::vector<const CompilationInputFile*> inputs;

public:
  DriverInputsAndOutputs() = default;
  DriverInputsAndOutputs(const DriverInputsAndOutputs &other);
  DriverInputsAndOutputs &operator=(const DriverInputsAndOutputs &other);

public:
  llvm::ArrayRef<const CompilationInputFile*> GetInputs() const { return inputs; }
  std::vector<std::string> GetCompilationInputFilenames() const;

  unsigned InputCount() const { return inputs.size(); }
  bool HasInputs() const { return !inputs.empty() && (InputCount() > 0); }
  bool HasNoInputs() const { return !HasInputs(); }
  bool HasSingleInput() const { return InputCount() == 1; }

  const CompilationInputFile* FirstInput() const {
    assert(HasInputs());
    return inputs.front();
  }
  //CompilationInputFile* FirstInput() {
  //   assert(HasInputs());
  //   return *(inputs.front());
  // }


  const CompilationInputFile* LastInput() const { return inputs.back(); }
  /// If \p fn returns true, exits early and returns true.
  void ForEachInput(std::function<void(const CompilationInputFile* )> fn) const;

public:
  void ClearInputs();
  void AddInput(const CompilationInputFile* input);
  void AddInput(llvm::StringRef file);
};

class DriverOptions final {

public:
  String defaultTarget;
};

} // namespace stone

#endif