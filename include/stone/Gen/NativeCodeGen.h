#ifndef STONE_GEN_NATIVECODEGEN_H
#define STONE_GEN_NATIVECODEGEN_H

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
class IRCodeGenResult;
namespace syn {
class Module;
class SyntaxContext;
} // namespace syn

class NativeCodeGen final {
  CodeGenContext &cgc;
  IRCodeGenResult &result;
  syn::SyntaxContext &sc;

public:
  NativeCodeGen(const NativeCodeGen &) = delete;
  void operator=(const NativeCodeGen &) = delete;

public:
  NativeCodeGen(CodeGenContext &cgc, IRCodeGenResult &result,
                syn::SyntaxContext &sc);
  ~NativeCodeGen();

public:
  void Emit();
};
} // namespace stone

#endif