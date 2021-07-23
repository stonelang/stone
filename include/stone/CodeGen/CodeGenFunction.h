#ifndef STONE_CODEGEN_CODEGENFUNCTION_H
#define STONE_CODEGEN_CODEGENFUNCTION_H

#include "stone/CodeGen/CodeEmitter.h"

namespace stone {
namespace codegen {

class CodeGenFunction final {
public:
  CodeEmitter emitter;

public:
  CodeEmitter &GetEmitter() { return emitter; }
};
} // namespace codegen
} // namespace stone

#endif
