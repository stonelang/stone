#ifndef STONE_COMPILE_COMPILEROPTIONS_H
#define STONE_COMPILE_COMPILEROPTIONS_H

#include "stone/Basic/ModuleOptions.h"
#include "stone/Compile/CompilerInputsAndOutputs.h"

#include "llvm/ADT/SmallString.h"
#include "llvm/Option/Arg.h"
#include "llvm/Option/ArgList.h"
#include "llvm/Option/Option.h"

namespace stone {

class CompilerInvocation;
class CompilerOptionsConverter;
class CompilerInputsConverter;

// using CompilerAction = unsigned;

enum class CompilerAction : unsigned {
  ///< No action
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
  ///< Merge modules only
  MergeModules,
  //< Parse, type-check, and emit assembly
  EmitAssembly,
};
class CompilerOptions final {

  friend CompilerInvocation;
  friend CompilerOptionsConverter;
  friend CompilerInputsConverter;

  /// The path the executing program
  llvm::StringRef mainExecutablePath;

  /// The name of the executing program
  llvm::StringRef mainExecutableName;

  llvm::SmallString<128> workingDirectory;

  CompilerAction mainAction = CompilerAction::None;

  /// The name of the executing program
  llvm::StringRef mainActionName;

public:
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
  /// \return true if the given action requires a proper module name
  static bool DoesActionNeedProperModuleName(CompilerAction action);
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
  /// \return the string name of the action
  static llvm::StringRef GetActionString(CompilerAction action);

public:
  /// \return true if this is any action.
  static bool IsAnyAction(CompilerAction action);

public:
  /// \return true if the action is valid
  bool HasMainAction() const {
    return CompilerOptions::IsAnyAction(GetMainAction());
  }

  /// \return the Action
  CompilerAction GetMainAction() const { return mainAction; }

  /// \return the name of the main action
  llvm::StringRef GetMainActionName() const { return mainActionName; }

  /// \check that there exist a working directory
  bool HasMainActionName() const {
    return !mainActionName.empty() && mainActionName.size() > 0;
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
  bool IsPrintASTBeforeAction() const;

  /// \return true if this is the TypeCheck action
  bool IsTypeCheckAction() const;

  /// \return true if this is the PrintASTAfter action
  bool IsPrintASTAfterAction() const;

  /// \return true if this is the EmitIRAfter action
  bool IsEmitIRAfterAction() const;

  /// \return true if this is the EmitIRBefore action
  bool IsEmitIRBeforeAction() const;

  /// \return true if the given action should generate IR
  bool DoesActionGenerateIR() const;

  /// \return true if the given action should generate native code
  bool DoesActionGenerateNative() const;

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