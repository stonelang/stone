#ifndef STONE_COMPILE_COMPILERINVOCATION_H
#define STONE_COMPILE_COMPILERINVOCATION_H

#include "stone/Compile/CompilerOptions.h"

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
  Status ParseCommandLine();

public:
  CompilerOptions &GetCompilerOptions() { return compilerOpts; }
  const CompilerOptions &GetCompilerOptions() const { return compilerOpts; }
};

} // namespace stone
#endif
