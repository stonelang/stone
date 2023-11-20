#ifndef STONE_COMPILE_COMPILE_H
#define STONE_COMPILE_COMPILE_H

#include "llvm/ADT/ArrayRef.h"

namespace stone {

class Status;
class CodeGenContext;
class CompilerListener;

int Compile(llvm::ArrayRef<const char *> args, const char *arg0, void *mainAddr,
            CompilerListener *listener = nullptr);

// Status CompileWithCodeAna(Compiler &compiler);
// Status CompileWithSyntaxAna(Compiler &compiler);
// Status CompileWithSemAna(Compiler &compiler);
// Status CompileWithCodeGen(Compiler &compiler);
// Status CompileWithGenIR(Compiler &compiler, CodeGenContext &codeGenContext);
// Status CompileWithGenNative(Compiler &compiler, CodeGenContext
// &codeGenContext);

} // namespace stone

#endif
