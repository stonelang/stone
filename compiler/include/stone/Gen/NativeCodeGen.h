#ifndef STONE_GEN_NATIVE_CODEGEN_H
#define STONE_GEN_NATIVE_CODEGEN_H

#include "stone/Basic/CodeGenOptions.h"
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

class ASTContext;

class NativeCodeGen final {

  const CodeGenOptions &codeGenOpts;
  ASTContext &astContext;
  // llvm::Optional<raw_fd_ostream> rawStream;
  /// outputbackend
public:
  NativeCodeGen(const NativeCodeGen &) = delete;
  void operator=(const NativeCodeGen &) = delete;

public:
  NativeCodeGen(const CodeGenOptions &codeGenOpts, ASTContext &astContext);
  ~NativeCodeGen();

  ASTContext &GetASTContext() { return astContext; }
  const CodeGenOptions &GetCodeGenOptions() const { return codeGenOpts; }

public:
  void CreateOutputFile();
  void OptimizeIR();
  void EmitNativeCode();
  void EmbedObject();
  void EmbedBitCode();
};
} // namespace stone

#endif