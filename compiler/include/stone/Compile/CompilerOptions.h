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

// TODO:
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
  DumpAST,
  ///< Parse and resolve imports only
  ResolveImports,
  ///< Parse and type-check only

  /// MARK -- Semantic analysis
  TypeCheck,
  ///< Parse, type-check, and  pretty print syntax tree
  PrintAST,

  // < This is a support action that the user will never see
  GenerateIR,
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