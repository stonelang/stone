#ifndef STONE_COMPILE_MODES_H
#define STONE_COMPILE_MODES_H

#include "stone/Compile/CompilableItem.h"

namespace stone {
namespace mode {

int Parse(CompilableItem &compilable, bool check);
int Parse(CompilableItem &compilable);
int Check(CompilableItem &compilable);
int EmitIR(CompilableItem &compilable);
int EmitObject(CompilableItem &compilable);
int EmitAssembly(CompilableItem &compilable);
int EmitLibrary(CompilableItem &compilable);
int EmitModule(CompilableItem &compilable);
int EmitBitCode(CompilableItem &compilable);

} // namespace mode
} // namespace stone
#endif