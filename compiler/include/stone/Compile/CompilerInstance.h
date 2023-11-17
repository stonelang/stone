#ifndef STONE_COMPILE_COMPILER_H
#define STONE_COMPILE_COMPILER_H

#include <deque>
#include <queue>

namespace stone {

class CompilerInvocation final {
public:
};
class CompilerInstance final {
public:
  CompilerInstance(CompilerInvocation &invocation) : invocation(invocation) {}
};

class CompilerExectution final {
  CompilerInstance &compiler;

public:
  CompilerExectution(CompilerInstance &compiler) : compiler(compiler) {}
};

} // namespace stone
#endif
