#ifndef STONE_COMPILE_COMPILERINVOCATION_H
#define STONE_COMPILE_COMPILERINVOCATION_H

#include "stone/Compile/CompilerOptions.h"


#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/SetVector.h"
#include "llvm/Option/ArgList.h"


namespace stone {

class Compiler;
class CompilerInvocation final {
  Compiler &compiler;
  CompilerOptions compilerOpts;

public:
  CompilerInvocation(Compiler &compiler) : compiler(compiler) {}

public:
  CompilerInvocation();

public:
  Status ParseCommandLine(llvm::ArrayRef<const char *> args);

public:
  CompilerOptions &GetCompilerOptions() { return compilerOpts; }
  const CompilerOptions &GetCompilerOptions() const { return compilerOpts; }
};

} // namespace stone
#endif
