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

enum class CompilerAction : unsigned {
  ///< No mode
  None = 0,
  /// MARK -- Lang support
  //< Print language version
  PrintVersion,
  ///< Print help
  PrintHelp,
  ///< Print help
  PrintHelpHidden,
  ///< Print compiler features
  PrintFeature,
  /// MARK -- Syntax analysis
  ///< Parse only
  Parse,
  ///< Parse and dump syntax tree
  PrintASTBefore,
  ///< Parse and resolve imports only
  ResolveImports,
  /// MARK -- Semantic analysis
  TypeCheck,
  ///< Parse, type-check, and  pretty print syntax tree
  PrintASTAfter,
  /// MARK -- Code generation
  //</ Parse, type-check, and emit LLVM IR pre optimization
  EmitIRBefore,
  //</ Parse, type-check, and emit LLVM IR post optimization
  EmitIRAfter,
  //</ Parse, type-check, and pretty print llvm-ir
  PrintIR,
  //< Parse, type-check, and emit LLVM BC
  EmitBC,
  ///< Parse, type-check, and emit native object code
  EmitObject,
  //< Parse, type-check, and emit a module. Ex: 'any.stonemod'
  EmitModule,
  //< Parse, type-check, and emit assembly
  EmitAssembly,
  ///< Invalid action
  Alien,
};
class CompilerOptions final {

  friend CompilerInvocation;
  friend CompilerOptionsConverter;
  friend CompilerInputsConverter;

  CompilerAction action = CompilerAction::None;

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

  /// The path to which we should output statistics files.
  std::string statsOutputDir;

  /// Trace changes to stats to files in StatsOutputDir.
  bool traceStats = false;

  /// Profile changes to stats to files in StatsOutputDir.
  bool profileEvents = false;

  /// Profile changes to stats to files in StatsOutputDir, grouped by source
  /// entity.
  bool profileEntities = false;

  /// The path the executing program
  llvm::StringRef mainExecutablePath;

  /// The name of the executing program
  llvm::StringRef mainExecutableName;

  llvm::SmallString<128> workingDirectory;

  // Some actions require just GenIR w/o actually emitting out.
  bool shouldEmitIR = false;

  enum class LibOutputMode {
    /// Default
    Dynamic = 0,
    /// "-static" option is set.
    Static
  };

  LibOutputMode libOutputMode = LibOutputMode::Dynamic;

  enum class ParsingInputMode {
    Stone,
    StoneLibrary,
    StoneModuleInterface,
  };
  ParsingInputMode parsingInputMode = ParsingInputMode::Stone;

public:
  /// \return true if the action is valid
  bool HasMainAction() const { return !mainAction.IsAlien(); }

  /// \return the Action
  Action &GetMainAction() { return mainAction; }

  /// \return the action
  const Action &GetMainAction() const { return mainAction; }

  CompilerInputsAndOutputs &GetInputsAndOutputs() { return inputsAndOutputs; }
  const CompilerInputsAndOutputs &GetInputsAndOutputs() const {
    return inputsAndOutputs;
  }

public:
  /// \return true if the given action only parses without doing other
  /// compilation steps.
  static bool ShouldActionOnlyParse(CompilerAction action);
  /// \return true if the given action should generates IR
  static bool DoesActionGenerateIR(CompilerAction action);
  /// \return true if the given action should generate native code
  static bool DoesActionGenerateNative(CompilerAction action);
  /// \return true if the given action requires the standard library to be
  /// loaded before it is run.
  static bool DoesActionRequireStoneStandardLibrary(CompilerAction action);
  /// \return true if the given action requires input files to be provided.
  static bool DoesActionRequireInputs(CompilerAction action);
  /// \return true if the given action produces output
  static bool DoesActionProduceOutput(CompilerAction action);
  /// \return true if the given action requires input files to be provided.
  static bool DoesActionPerformEndOfPipelineActions(CompilerAction action);
  /// \return true if the given action supports caching.
  static bool DoesActionSupportCompilationCaching(CompilerAction action);
  /// \return the FileType for the action
  static file::FileType GetActionOutputFileType(CompilerAction action);

public:
  /// \return true if this is any action.
  static bool IsAnyAction(CompilerAction action);

public:
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

  /// \return true if this is the ResolveImports action
  bool IsResolveImportsAction() const;

  /// \return true if this is the PrintASTBefore action
  bool IsPrintASTBeforeAction() const;

  /// \return true if this is the TypeCheck action
  bool IsTypeCheckAction() const;

  /// \return true if this is the PrintASTAfter action
  bool IsPrintASTAfterAction() const;

  /// \return true if this is the EmitIRAfter action
  bool IsEmitIRAfterAction() const;

  /// \return true if this is the EmitIRBefore action
  bool IsEmitIRBeforeAction() const;

  /// \return true if this is the EmitModule action
  bool IsEmitModuleAction() const;

  /// \return true if this is the EmitBC action
  bool IsEmitBCAction() const;

  /// \return true if this is the EmitObject action
  bool IsEmitObjectAction() const;

  /// \return true if this is the EmitAssembly action
  bool IsEmitAssemblyAction() const;
};

} // namespace stone

#endif