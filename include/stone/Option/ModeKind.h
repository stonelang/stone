#ifndef STONE_OPTION_MODEKIND_H
#define STONE_OPTION_MODEKIND_H

namespace stone {

// The modes that the system supports
enum class ModeKind : unsigned {
  ///< No mode
  None = 0,
  ///< Parse only
  Parse,
  ///< Parse and emit syntax
  EmitParse,
  ///< Parse and type-check only
  TypeCheck,
  ///< Parse, type-check, and emit syntax
  EmitSyntax,
  //</ Parse, type-check, and emit LLVM IR
  EmitIR,
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
  //< Print language version
  PrintVersion,
  ///< Print help
  PrintHelp,
  //< Alien
  Alien,
};
} // namespace stone
#endif
