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

class CodeGenMachine final {
  CodeGenContext &cgc;
  syn::SyntaxContext &sc;

  // llvm::Optional<raw_fd_ostream> rawStream;
public:
  CodeGenMachine(const CodeGenMachine &) = delete;
  void operator=(const CodeGenMachine &) = delete;

public:
  CodeGenMachine(CodeGenContext &cgc, syn::SyntaxContext &sc);
  ~CodeGenMachine();

  CodeGenContext &GetCodeGenContext() { return cgc; }
  syn::SyntaxContext &GetSyntaxContext() { return sc; }
};
} // namespace stone

#endif