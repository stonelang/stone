#ifndef STONE_COMPILE_COMPILEROPTIONS_H
#define STONE_COMPILE_COMPILEROPTIONS_H

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
  Action MainAction;

  /// A list of arbitrary modules to import and make implicitly visible.
  std::vector<std::pair<String, bool /*testable*/>> implicitModuleNames;

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
  llvm::StringRef MainExecutablePath;

  /// The name of the executing program
  llvm::StringRef MainExecutableName;

  llvm::SmallString<128> workingDirectory;

  enum class LibOutputMode { Dynamic, Static };
  LibOutputMode libOutputMode = LibOutputMode::Dynamic;

  enum class ParsingInputMode {
    Stone,
    StoneLibrary,
    StoneModuleInterface,
  };
  ParsingInputMode parsingInputMode = ParsingInputMode::Stone;
};

} // namespace stone

#endif