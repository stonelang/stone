#ifndef STONE_COMPILE_COMPILEROPTIONS_H
#define STONE_COMPILE_COMPILEROPTIONS_H

#include "stone/AST/Module.h"
#include "stone/Compile/CompilerInputsAndOutputs.h"
#include "stone/Support/Options.h"

#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/Option/Arg.h"
#include "llvm/Option/ArgList.h"
#include "llvm/Option/Option.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/Path.h"

#include <stdint.h>
#include <string>

namespace stone {
class CompilerInvocation;
class CompilerOptionsConverter;
class CompilerInputsConverter;

enum class CompilerActionKind : uint8_t {
#define COMPILER_MODE(A) A,
#include "stone/Support/ActionKind.def"
};

class CompilerOptions final : public Options {

  friend CompilerInvocation;
  friend CompilerOptionsConverter;
  friend CompilerInputsConverter;

  /// The name of the executing program
  llvm::StringRef primaryActionName;

  CompilerActionKind primaryActionKind = CompilerActionKind::None;

public:
  bool traceAction = false;

  CompilerInputsAndOutputs inputsAndOutputs;

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
  bool downgradeInterfaceVerificationError = false;

  /// The path to which we should output statistics files.
  std::string statsOutputDir;

  /// Follow the compile lifecyle
  bool printLifecycle = false;

  /// Trace changes to stats to files in StatsOutputDir.
  bool traceStats = false;

  /// Profile changes to stats to files in StatsOutputDir.
  bool profileEvents = false;

  /// Profile changes to stats to files in StatsOutputDir, grouped by source
  /// entity.
  bool profileEntities = false;

  // Some actions require just GenIR w/o actually emitting out.
  bool shouldEmitIR = false;

  bool noIROptimization = false;

  // enum class LibOutputMode {
  //   /// Default
  //   Dynamic = 0,
  //   /// "-static" option is set.
  //   Static
  // };

  // LibOutputMode libOutputMode = LibOutputMode::Dynamic;

  enum class ParsingInputMode {
    Stone,
    StoneLibrary,
    StoneModuleInterface,
  };
  ParsingInputMode parsingInputMode = ParsingInputMode::Stone;

public:
  CompilerOptions();

public:
  /// \check that there exist a working directory
  bool HasWorkingDirectory() const {
    return !workingDirectory.empty() && workingDirectory.size() > 0;
  }
  /// \return working directory for the compilation
  llvm::StringRef GetWorkingDirectory() const { return workingDirectory; }

  /// \return the main executable path
  llvm::StringRef GetMainExecutablePath() const { return mainExecutablePath; }

  /// \check that there exist the main executable path
  bool HasMainExecutablePath() const {
    return !mainExecutablePath.empty() && mainExecutablePath.size() > 0;
  }
  /// \return the main executable name
  llvm::StringRef GetMainExecutableName() const { return mainExecutableName; }

  /// \check that there exist the main executable path
  bool HasMainExecutableName() const {
    return !mainExecutableName.empty() && mainExecutableName.size() > 0;
  }

  CompilerInputsAndOutputs &GetInputsAndOutputs() { return inputsAndOutputs; }
  const CompilerInputsAndOutputs &GetInputsAndOutputs() const {
    return inputsAndOutputs;
  }

public:
  /// \return true if the action is valid
  bool HasPrimaryAction() const {
    return CompilerOptions::IsAnyAction(GetPrimaryAction());
  }
  /// \return the Action
  CompilerActionKind GetPrimaryAction() const { return primaryActionKind; }
  /// \return the name of the main action
  llvm::StringRef GetPrimaryActionName() const { return primaryActionName; }
  /// \check that there exist a working directory
  bool HasPrimaryActionName() const {
    return !primaryActionName.empty() && primaryActionName.size() > 0;
  }

  /// \return true if this is the None action
  bool IsNoneAction() const;

  /// \return true if this is the PrintHelp action
  bool IsPrintHelpAction() const;

  /// \return true if this is the PrintHelpHidden action.
  bool IsPrintHelpHiddenAction() const;

  /// \return true if this is the PrintVersion action
  bool IsPrintVersionAction() const;

  /// \return true if this is the PrintFeature action
  bool IsPrintFeatureAction() const;

  /// \return true if this is the Parse action
  bool IsParseAction() const;

  /// \return true if you are only parsing
  bool ShouldActionOnlyParse() const;

  /// \return true if this is the ResolveImports action
  bool IsResolveImportsAction() const;

  /// \return true if this is the PrintASTBefore action
  bool IsEmitParseAction() const;

  /// \return true if this is the TypeCheck action
  bool IsTypeCheckAction() const;

  /// \return true if this is the PrintASTAfter action
  bool IsEmitASTAction() const;

  /// \return true if this is the EmitIRAfter action
  bool IsEmitIRAction() const;

  /// \return true if the given action should generate IR
  bool DoesActionGenerateIR() const;

  /// \return true if the given action should generate native code
  bool DoesActionGenerateNativeCode() const;

  /// \return true if this is the EmitModule action
  bool IsEmitModuleAction() const;

  /// \return true if this is the EmitBC action
  bool IsEmitBCAction() const;

  /// \return true if this is the EmitObject action
  bool IsEmitObjectAction() const;

  /// \return true if this is the EmitAssembly action
  bool IsEmitAssemblyAction() const;

  /// \return true if this is an immediate action
  bool IsImmediateAction() const;

  /// \return true if this is any action
  bool IsAction(CompilerActionKind k) const { return GetPrimaryAction() == k; }

  template <typename... T>
  bool IsAnyAction(CompilerActionKind K1, CompilerActionKind K2, T... K) const {
    if (IsAction(K1)) {
      return true;
    }
    return IsAnyAction(K2, K...);
  }

public:
  /// \return true if the given action requires a proper module name
  static bool DoesActionNeedProperModuleName(CompilerActionKind actionKind);
  /// \return true if the given action only parses without doing other
  /// compilation steps.
  static bool ShouldActionOnlyParse(CompilerActionKind actionKind);
  /// \return true if the given action should generates IR
  static bool DoesActionGenerateIR(CompilerActionKind actionKind);
  /// \return true if the given action should generate native code
  static bool DoesActionGenerateNativeCode(CompilerActionKind actionKind);
  /// \return true if the given action requires the standard library to be
  /// loaded before it is run.
  static bool
  DoesActionRequireStoneStandardLibrary(CompilerActionKind actionKind);
  /// \return true if the given action requires input files to be provided.
  static bool DoesActionRequireInputs(CompilerActionKind actionKind);
  /// \return true if the given action produces output
  static bool DoesActionProduceOutput(CompilerActionKind actionKind);
  /// \return true if the given action requires input files to be provided.
  static bool
  DoesActionPerformEndOfPipelineActions(CompilerActionKind actionKind);
  /// \return true if the given action supports caching.
  static bool
  DoesActionSupportCompilationCaching(CompilerActionKind actionKind);
  /// \return the FileType for the action
  static stone::FileType GetActionOutputFileType(CompilerActionKind actionKind);
  /// \return the string name of the action
  static llvm::StringRef GetActionString(CompilerActionKind actionKind);
  /// \return true if this is any action.
  static bool IsAnyAction(CompilerActionKind actionKind);

  void ForAllOutputPaths(const CompilerInputFile &input,
                         llvm::function_ref<void(StringRef)> fn) const;

  bool IsOutputFileDirectory() const;

public:
  static Status IsValidModuleName(const llvm::StringRef moduleName);
};

} // namespace stone

#endif