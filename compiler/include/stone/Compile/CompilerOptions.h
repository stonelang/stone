#ifndef STONE_COMPILE_LANGOPTIONS_H
#define STONE_COMPILE_LANGOPTIONS_H

#include "stone/Basic/FileSystemOptions.h"
#include "stone/Basic/LangOptions.h"
#include "stone/Basic/ModuleOptions.h"
#include "stone/Basic/SrcLoc.h"
#include "stone/Basic/TargetOptions.h"
#include "stone/Compile/CompilerInputsAndOutputs.h"
#include "stone/Session/BaseOptions.h"
#include "stone/Session/Options.h"
#include "stone/Syntax/SearchPath.h"

namespace stone {

class CompilerOptions final : public BaseOptions {

  friend class CompilerInvocation;
  /// A list of arbitrary modules to import and make implicitly visible.
  std::vector<std::pair<std::string, bool /*testable*/>> implicitModuleNames;

  ///
  CompilerInputsAndOutputs inputsAndOutputs;

public:
  enum class ThreadModelKind {
    /// POSIX Threads.
    POSIX,
    /// Single Threaded Environment.
    Single
  };

  ///
  ThreadModelKind threadModelKind = ThreadModelKind::POSIX;

  ModuleOptions moduleOpts;

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

  enum class LibOutputMode { Dynamic, Static };
  LibOutputMode libOutputMode = LibOutputMode::Dynamic;

  enum class InputFileMode {
    Stone,
    StoneLibrary,
    StoneModuleInterface,
  };
  InputFileMode inputFileMode = InputFileMode::Stone;

public:
  CompilerOptions(std::unique_ptr<Mode> mode) : BaseOptions(std::move(mode)) {
    GetCompilerInputsAndOutputs().ClearInputs();
  }

public:
  CompilerInputsAndOutputs &GetCompilerInputsAndOutputs() {
    return inputsAndOutputs;
  }
};

} // namespace stone
#endif
