#ifndef STONE_GEN_IRCODEGEN_H
#define STONE_GEN_IRCODEGEN_H

#include "stone/Basic/LLVM.h"
#include "stone/Gen/CodeGenContext.h"
#include "stone/Gen/IRCodeGenBuilder.h"
#include "stone/Gen/IRCodeGenTypeCache.h"
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

class CodeGenListener;
class IRCodeGen final {
  CodeGenContext &cgc;
  CodeGenListener *listener;

public:
  IRCodeGen(CodeGenContext &cgc, CodeGenListener *listener = nullptr);
  ~IRCodeGen();
  // IRCodeGenFunction &GetIRCodeGenFunction();

  CodeGenContext &GetCodeGenContext() { return cgc; }
  CodeGenListener *GetCodeGenListener() { return listener; }

public:
  Safe<llvm::TargetMachine> CreateTargetMachine();
};
} // namespace stone

#endif