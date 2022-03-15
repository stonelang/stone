#ifndef STONE_GEN_OBJCODE_H
#define STONE_GEN_OBJCODE_H

#include "stone/Gen/CodeGenContext.h"

namespace llvm {
class raw_pwrite_stream;
class GlobalVariable;
class MemoryBuffer;
class Module;
class TargetOptions;
class TargetMachine;
} // namespace llvm

namespace stone {
namespace syn {
class SyntaxContext;
class Module;
} // namespace syn

class ObjGen final {
  CodeGenContext &codeGenContext;
  llvm::Module *llvmMod = nullptr;
  syn::SyntaxContext &tc;

public:
  ObjGen(const ObjGen &) = delete;
  void operator=(const ObjGen &) = delete;

public:
  ObjGen(CodeGenContext &codeGenContext, llvm::Module *llvmMod,
         syn::SyntaxContext &tc);
  ~ObjGen();

public:
  void Emit();
};
} // namespace stone

#endif