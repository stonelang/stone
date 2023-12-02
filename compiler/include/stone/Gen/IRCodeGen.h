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
class IRCodeGen final {
  const CodeGenOptions &codeGenOpts;
  ASTContext &astContext;
  // llvm::Optional<raw_fd_ostream> rawStream;
  /// outputbackend
public:
  IRCodeGen(const IRCodeGen &) = delete;
  void operator=(const IRCodeGen &) = delete;

public:
  IRCodeGen() = delete;
  IRCodeGen(const CodeGenOptions &codeGenOpts,
            ASTContext &astContext) = default;

  ~IRCodeGen();

  ASTContext &GetASTContext() { return astContext; }
  const CodeGenOptions &GetCodeGenOptions() const { return codeGenOpts; }

public:
  llvm::TargetMachine &GetTargetMachine();
};
} // namespace stone

#endif