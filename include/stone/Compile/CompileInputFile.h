#ifndef STONE_COMPILE_COMPILEINPUTFILE_H
#define STONE_COMPILE_COMPILEINPUTFILE_H

#include "stone/Compile/CompilableItem.h"
#include "llvm/ADT/ArrayRef.h"

#include <memory>

namespace stone {

class Compiler;
std::unique_ptr<CompilableItem> CompileInputFile(Compiler &compiler, file::File &input);

int Parse(CompilableItem &compilable, bool check);
int Parse(CompilableItem &compilable);
int Check(CompilableItem &compilable);
int EmitIR(CompilableItem &compilable);
int EmitObject(CompilableItem &compilable);
int EmitAssembly(CompilableItem &compilable);
int EmitLibrary(CompilableItem &compilable);
int EmitModule(CompilableItem &compilable);
int EmitBitCode(CompilableItem &compilable);

} // namespace stone
#endif
