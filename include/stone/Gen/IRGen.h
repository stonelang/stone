#ifndef STONE_GEN_IRCODE_H
#define STONE_GEN_IRCODE_H

#include <memory>

#include "stone/Core/LLVM.h"
#include "stone/Gen/CodeGenContext.h"
#include "stone/Gen/IRFunction.h"
#include "stone/Gen/IRModule.h"
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

class IRLoop {};
class IRCall {};

class IRGen final {
  CodeGenContext &cgc;

public:
  IRGen() = delete;
  IRGen(CodeGenContext &cgc);
  ~IRGen();

public:
  /// Return an opaque reference to the IRCodeModule object, which can
  /// be used in various secondary APIs.  It is valid as long as the
  /// Generator exists.
  // IRModule &GetIRModule();
  // IRFunction &GetIRFunction();

  CodeGenContext &GetCodeGenContext() { return cgc; }
};
} // namespace stone

#endif