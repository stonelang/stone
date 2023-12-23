#ifndef STONE_GEN_NATIVE_CODEGEN_H
#define STONE_GEN_NATIVE_CODEGEN_H

#include "stone/Basic/CodeGenOptions.h"

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

class NativeGen final {

  const CodeGenOptions &codeGenOpts;
  ASTContext &astContext;
  // llvm::Optional<raw_fd_ostream> rawStream;
  /// outputbackend
public:
  NativeGen(const NativeGen &) = delete;
  void operator=(const NativeGen &) = delete;

public:
  NativeGen(const CodeGenOptions &codeGenOpts, ASTContext &astContext);
  ~NativeGen();

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