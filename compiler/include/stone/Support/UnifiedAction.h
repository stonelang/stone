#ifndef STONE_SUPPORT_UNIFIEDACTION_H
#define STONE_SUPPORT_UNIFIEDACTION_H

namespace stone {
enum class UnifiedAction : unsigned {
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
  DumpAST,
  ///< Parse and resolve imports only
  ResolveImports,
  /// MARK -- Semantic analysis
  TypeCheck,
  ///< Parse, type-check, and  pretty print syntax tree
  PrintAST,
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
  ///< Parse, type-check, and emit a library.
  ///< Default => platform specific. But, with -static => 'any.a'
  EmitLibrary,
  //< Parse, type-check, and emit a module. Ex: 'any.stonemod'
  EmitModule,
  //< Parse, type-check, and emit assembly
  EmitAssembly,
  ///< Merge all modules
  MergeModules,
};

  /// \return true if the given action requires a proper module name
   bool DoesActionNeedProperModuleName(UnifiedAction action);
  /// \return true if the given action only parses without doing other
  /// compilation steps.
   bool ShouldActionOnlyParse(UnifiedAction action);
  /// \return true if the given action should generates IR
   bool DoesActionGenerateIR(UnifiedAction action);
  /// \return true if the given action should generate native code
   bool DoesActionGenerateNative(UnifiedAction action);
  /// \return true if the given action requires the standard library to be
  /// loaded before it is run.
   bool DoesActionRequireStoneStandardLibrary(UnifiedAction action);
  /// \return true if the given action requires input files to be provided.
   bool DoesActionRequireInputs(UnifiedAction action);
  /// \return true if the given action produces output
   bool DoesActionProduceOutput(UnifiedAction action);
  /// \return true if the given action requires input files to be provided.
   bool DoesActionPerformEndOfPipelineActions(UnifiedAction action);
  /// \return true if the given action supports caching.
   bool DoesActionSupportCompilationCaching(UnifiedAction action);
  /// \return the FileType for the action
   file::FileType GetActionOutputFileType(UnifiedAction action);
  /// \return the string name of the action
   llvm::StringRef GetActionString(UnifiedAction action);

} // namespace stone
#endif
