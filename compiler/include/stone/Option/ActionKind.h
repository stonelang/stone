#ifndef STONE_OPTION_ACTIONKIND_H
#define STONE_OPTION_ACTIONKIND_H

namespace stone {

// TODO: May want DumpParse, and DumpySyntax
// The modes that the system supports
enum class ActionKind : unsigned {
  ///< No mode
  None = 0,
  //< Print language version
  PrintVersion,
  ///< Print help
  PrintHelp,
  ///< Print help
  PrintHelpHidden,
  ///< Print compiler features
  PrintFeature,
  ///< Parse only
  Parse,
  ///< Parse and resolve use(s) only
  ResolveImports,
  ///< Parse and dump syntax tree
  DumpSyntax,
  ///< Parse and type-check only
  TypeCheck,
  ///< TODO
  DumpTypeInfo,
  ///< Parse, type-check, and  pretty print syntax tree
  PrintSyntax,
  //</ Parse, type-check, and pretty print llvm-ir
  PrintIR,
  //</ Parse, type-check, and emit LLVM IR pre optimization
  EmitIRBefore,
  //</ Parse, type-check, and emit LLVM IR post optimization
  EmitIRAfter,
  //< Parse, type-check, and emit LLVM BC
  EmitBC,
  ///< Parse, type-check, and emit native object code
  EmitObject,
  ///< Parse, type-check, and emit a library.
  ///< Default => platform specific. But, with -static => 'any.a'
  EmitLibrary,
  ///< Create a module fule
  InitModule,
  //< Parse, type-check, and emit a module. Ex: 'any.stonemod'
  EmitModule,
  //< Parse, type-check, and emit assembly
  EmitAssembly,
  ///< Merge all modules
  MergeModules,
  //< Alien
  Alien,
};
} // namespace stone
#endif
