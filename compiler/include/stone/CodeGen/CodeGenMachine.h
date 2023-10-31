#ifndef STONE_GEN_BACKENDCODEGEN_H
#define STONE_GEN_BACKENDCODEGEN_H

#include "stone/CodeGen/CodeGenContext.h"
#include "stone/Lang.h"

namespace llvm {
class raw_pwrite_stream;
class GlobalVariable;
class MemoryBuffer;
class Module;
class TargetOptions;
class TargetMachine;
} // namespace llvm

namespace stone {
namespace ast {
class Module;
class ASTContext;
} // namespace ast

class CodeGenMachine final {
  CodeGenContext &cgc;
  ast::ASTContext &sc;

  // llvm::Optional<raw_fd_ostream> rawStream;
public:
  CodeGenMachine(const CodeGenMachine &) = delete;
  void operator=(const CodeGenMachine &) = delete;

public:
  CodeGenMachine(CodeGenContext &cgc, ast::ASTContext &sc);
  ~CodeGenMachine();

  CodeGenContext &GetCodeGenContext() { return cgc; }
  ast::ASTContext &GetASTContext() { return sc; }
};
} // namespace stone

#endif