#ifndef STONE_COMPILE_COMPILERSOURCEFILEENGINE_H
#define STONE_COMPILE_COMPILERSOURCEFILEENGINE_H

namespace stone {
class Compiler;

class CompilerSourceSystem final {
  Compiler &compiler;

public:
  CompilerSourceSystem(Compiler &compiler);
};

} // namespace stone

#endif