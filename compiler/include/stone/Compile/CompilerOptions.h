#ifndef STONE_COMPILE_COMPILEROPTIONS_H
#define STONE_COMPILE_COMPILEROPTIONS_H

#include "stone/Basic/ModuleOptions.h"
#include "stone/Compile/CompilerInputsAndOutputs.h"
#include "stone/Option/Action.h"

#include "llvm/ADT/SmallString.h"

namespace stone {

class CompilerInvocation;
class CompilerOptionsConverter;
class CompilerInputsConverter;

class CompilerOptions final {

  friend CompilerInvocation;
  friend CompilerOptionsConverter;
  friend CompilerInputsConverter;

public:
  /// The main action requested.
  Action mainAction;

  bool trackAction = false;

  ModuleOptions moduleOpts;

  CompilerInputsAndOutputs inputsAndOutputs;

  /// Indicates that the input(s) should be parsed as the Stone stdlib.
  bool shouldParseAsStdLib = false;

  ///
  bool shouldProcessDuplicateInputFile = false;

  /// Best effort to output a .swiftmodule regardless of any compilation
  /// errors. SIL generation and serialization is skipped entirely when there
  /// are errors. The resulting serialized AST may include errors types and
  /// skip nodes entirely, depending on the errors involved.
  bool allowModuleWithCompilerErrors = false;

  /// Downgrade all errors emitted in the module interface verification phase
  /// to warnings.
  /// TODO: remove this after we fix all project-side warnings in the interface.
  bool downgradeInterfaceVerificationError = false;

  /// The path the executing program
  llvm::StringRef mainExecutablePath;

  /// The name of the executing program
  llvm::StringRef mainExecutableName;

  llvm::SmallString<128> workingDirectory;

  enum class LibOutputMode { Dynamic, Static };
  LibOutputMode libOutputMode = LibOutputMode::Dynamic;

  enum class ParsingInputMode {
    Stone,
    StoneLibrary,
    StoneModuleInterface,
  };
  ParsingInputMode parsingInputMode = ParsingInputMode::Stone;

  CompilerInputsAndOutputs &GetInputsAndOutputs() { return inputsAndOutputs; }
  const CompilerInputsAndOutputs &GetInputsAndOutputs() const {
    return inputsAndOutputs;
  }
};

} // namespace stone

#endif