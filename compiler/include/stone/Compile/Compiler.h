#ifndef STONE_COMPILE_COMPILER_H
#define STONE_COMPILE_COMPILER_H

#include "stone/Compile/CompilerOptions.h"

#include <deque>

namespace stone {

class CompilerInvocation final {

  CompilerOptions compilerOpts;

public:
  CompilerInvocation();

public:
  CompilerOptions &GetCompilerOptions() { return compilerOpts; }
  const CompilerOptions &GetCompilerOptions() const { return compilerOpts; }
};

class Compiler final {
public:
  Compiler(CompilerInvocation &invocation) : invocation(invocation) {}
};

class CompilerExectution final {
  Compiler &compiler;

public:
  CompilerExectution(Compiler &compiler) : compiler(compiler) {}

public:
  Status ExecuteParseOnly();
  Status ExecuteParseAndResolveImports();
};

} // namespace stone
#endif
