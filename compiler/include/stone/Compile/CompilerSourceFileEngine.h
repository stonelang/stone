#ifndef STONE_COMPILE_COMPILERSOURCEFILEENGINE_H
#define STONE_COMPILE_COMPILERSOURCEFILEENGINE_H

namespace stone {
class Compiler;

class CompilerSourceFileEngine final {
  Compiler &compiler;

public:
  CompilerSourceFileEngine(Compiler &compiler);
};

} // namespace stone

#endif