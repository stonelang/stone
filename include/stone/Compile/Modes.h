#ifndef STONE_COMPILE_MODES_H
#define STONE_COMPILE_MODES_H

#include "stone/Compile/CompilableItem.h"

namespace stone {
namespace mode {
int Execute(CompilableItem &ci);
int Parse(CompilableItem &ci, bool check);
int Parse(CompilableItem &ci);
int Check(CompilableItem &ci);
int EmitIR(CompilableItem &ci);
int EmitObject(CompilableItem &ci);
int EmitAssembly(CompilableItem &ci);
int EmitLibrary(CompilableItem &ci);
int EmitModule(CompilableItem &ci);
int EmitBitCode(CompilableItem &ci);

} // namespace mode
} // namespace stone
#endif