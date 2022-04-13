#ifndef STONE_GEN_IRCODEGENBUILDER_H
#define STONE_GEN_IRCODEGENBUILDER_H

#include "stone/Basic/LLVM.h"
#include "stone/Gen/CodeGenContext.h"
#include "stone/Syntax/Module.h"
#include "llvm/IR/PassManager.h"

#include <memory>

namespace llvm {
class raw_pwrite_stream;
class GlobalVariable;
class MemoryBuffer;
class Module;
class TargetOptions;
class TargetMachine;
} // namespace llvm

namespace stone {

class IRCodeGenBuilder final {
  CodeGenContext &cgc;

public:
  IRCodeGenBuilder(CodeGenContext &cgc);
  ~IRCodeGenBuilder();
};
} // namespace stone

#endif