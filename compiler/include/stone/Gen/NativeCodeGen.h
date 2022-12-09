#ifndef STONE_GEN_NATIVECODEGEN_H
#define STONE_GEN_NATIVECODEGEN_H

#include "stone/Gen/CodeGenContext.h"
#include "stone/Public.h"

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

class NativeCodeGen final {
  CodeGenContext &cgc;
  syn::SyntaxContext &sc;

public:
  NativeCodeGen(const NativeCodeGen &) = delete;
  void operator=(const NativeCodeGen &) = delete;

public:
  NativeCodeGen(CodeGenContext &cgc, syn::SyntaxContext &sc);
  ~NativeCodeGen();

  CodeGenContext &GetCodeGenContext() { return cgc; }
  syn::SyntaxContext &GetSyntaxContext() { return sc; }

  // public:
  //   void EmitObject(const CodeGenContext &result);
  //   void EmitBC(const  CodeGenContext &result);
  //   void EmitAssembly(const CodeGenContext &result);
};
} // namespace stone

#endif