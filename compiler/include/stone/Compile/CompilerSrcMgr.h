#ifndef STONE_COMPILE_COMPILER_SRC_MGR_H
#define STONE_COMPILE_COMPILER_SRC_MGR_H

namespace stone {
class Compiler;

class CompilerSrcMgr final {
  Compiler &compiler;

public:
  CompilerSrcMgr(Compiler &compiler);
};

} // namespace stone

#endif