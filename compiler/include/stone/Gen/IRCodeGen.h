#ifndef STONE_GEN_IRCODEGEN_H
#define STONE_GEN_IRCODEGEN_H

#include <memory>

#include "stone/Basic/LLVM.h"
#include "stone/Gen/CodeGenContext.h"
#include "stone/Gen/IRCodeGenBuilder.h"
#include "stone/Syntax/Module.h"

#include "llvm/IR/PassManager.h"

namespace llvm {
class raw_pwrite_stream;
class GlobalVariable;
class MemoryBuffer;
class Module;
class TargetOptions;
class TargetMachine;
} // namespace llvm

namespace stone {

// class IREmitterLoop {};
// class IREmitterCall {};

// class IRCodeBlocks {
// public:
//   IREmitterBlocks();
// };

class IRCodeGen final {
  CodeGenContext &cgc;
  IRCodeGenBuilder irCodeGenBuilder;

  // Safe<IRModuleResult> result;
public:
  IRCodeGen(CodeGenContext &cgc);
  ~IRCodeGen();
  // IRCodeGenFunction &GetIRCodeGenFunction();

  CodeGenContext &GetCodeGenContext() { return cgc; }
  IRCodeGenBuilder &GetIRCodeGenBuilder() { return irCodeGenBuilder; }
};
} // namespace stone

#endif