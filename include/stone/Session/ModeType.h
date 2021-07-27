#ifndef STONE_SESSION_MODEKIND_H
#define STONE_SESSION_MODEKIND_H

namespace stone {

enum class ModeType {
  None,
  Parse,
  TypeCheck,
  EmitIR,
  EmitBC,
  EmitObject,
  EmitLibrary,
  EmitModule,
  EmitAssembly,
  EmitExecutable,
  Link
};
}
#endif
