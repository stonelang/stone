#ifndef STONE_GEN_BACKENDCODEGEN_H
#define STONE_GEN_BACKENDCODEGEN_H

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

class BackendCodeGen final {
  CodeGenContext &cgc;
  syn::SyntaxContext &sc;

public:
  BackendCodeGen(const BackendCodeGen &) = delete;
  void operator=(const BackendCodeGen &) = delete;

public:
  BackendCodeGen(CodeGenContext &cgc, syn::SyntaxContext &sc);
  ~BackendCodeGen();

  CodeGenContext &GetCodeGenContext() { return cgc; }
  syn::SyntaxContext &GetSyntaxContext() { return sc; }

  // public:
  //   void EmitObject(const CodeGenContext &result);
  //   void EmitBC(const  CodeGenContext &result);
  //   void EmitAssembly(const CodeGenContext &result);
};
} // namespace stone

#endif