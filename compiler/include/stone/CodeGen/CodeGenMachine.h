#ifndef STONE_GEN_BACKENDCODEGEN_H
#define STONE_GEN_BACKENDCODEGEN_H

#include "stone/CodeGen/CodeGenContext.h"
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
class ASTContext;
} // namespace syn

class CodeGenMachine final {
  CodeGenContext &cgc;
  syn::ASTContext &sc;

  // llvm::Optional<raw_fd_ostream> rawStream;
public:
  CodeGenMachine(const CodeGenMachine &) = delete;
  void operator=(const CodeGenMachine &) = delete;

public:
  CodeGenMachine(CodeGenContext &cgc, syn::ASTContext &sc);
  ~CodeGenMachine();

  CodeGenContext &GetCodeGenContext() { return cgc; }
  syn::ASTContext &GetASTContext() { return sc; }
};
} // namespace stone

#endif