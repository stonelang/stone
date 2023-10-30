#ifndef STONE_COMPILE_LANGOPTIONS_H
#define STONE_COMPILE_LANGOPTIONS_H

#include "stone/Basic/FileSystemOptions.h"
#include "stone/Basic/LangOptions.h"
#include "stone/Basic/ModuleOptions.h"
#include "stone/Basic/STDAlias.h"
#include "stone/Basic/SrcLoc.h"
#include "stone/Basic/TargetOptions.h"
#include "stone/Compile/CompilerInputsAndOutputs.h"
#include "stone/Session/Options.h"
#include "stone/Session/SessionOptions.h"
#include "stone/AST/SearchPath.h"

namespace stone {

class CompilerOptions final {

  friend class CompilerInvocation;
  friend class CompilerOptionsConverter;
  friend class CompilerInputsConverter;

  std::unique_ptr<Mode> mode;

  /// A list of arbitrary modules to import and make implicitly visible.
  Vector<Pair<String, bool /*testable*/>> implicitModuleNames;

  ///
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
  bool DowngradeInterfaceVerificationError = false;

public:
  enum class LibOutputMode { Dynamic, Static };
  LibOutputMode libOutputMode = LibOutputMode::Dynamic;

  enum class ParsingInputMode {
    Stone,
    StoneLibrary,
    StoneModuleInterface,
  };
  ParsingInputMode parsingInputMode = ParsingInputMode::Stone;

public:
  CompilerOptions(std::unique_ptr<Mode> mode) : mode(std::move(mode)) {
    GetInputsAndOutputs().ClearInputs();
  }

public:
  CompilerInputsAndOutputs &GetInputsAndOutputs() { return inputsAndOutputs; }
  const CompilerInputsAndOutputs &GetInputsAndOutputs() const {
    return inputsAndOutputs;
  }
};

} // namespace stone
#endif
