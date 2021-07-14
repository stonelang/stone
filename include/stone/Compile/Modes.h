#ifndef STONE_COMPILE_MODES_H
#define STONE_COMPILE_MODES_H

#include "stone/Compile/CompilableItem.h"

namespace stone {
namespace mode {

static int Parse(CompilableItem &compilable, bool check);
static int Parse(CompilableItem &compilable);
static int Check(CompilableItem &compilable);
static int EmitIR(CompilableItem &compilable);
static int EmitObject(CompilableItem &compilable);
static int EmitAssembly(CompilableItem &compilable);
static int EmitLibrary(CompilableItem &compilable);
static int EmitModule(CompilableItem &compilable);
static int EmitBitCode(CompilableItem &compilable);

} // namespace mode
} // namespace stone
#endif