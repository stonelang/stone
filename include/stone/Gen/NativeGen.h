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
class Module;
class SyntaxContext;
} // namespace syn

class NativeGen final {
  CodeGenContext &cgc;
  llvm::Module *llvmMod = nullptr;
  syn::SyntaxContext &tc;

public:
  NativeGen(const NativeGen &) = delete;
  void operator=(const NativeGen &) = delete;

public:
  NativeGen(CodeGenContext &cgc, llvm::Module *llvmMod, syn::SyntaxContext &tc);
  ~NativeGen();

public:
  void Emit();
};
} // namespace stone

#endif