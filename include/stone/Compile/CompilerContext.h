#ifndef STONE_COMPILE_COMPILERCONTEXT_H
#define STONE_COMPILE_COMPILERCONTEXT_H

#include "stone/Basic/List.h"
#include "stone/Compile/CompilableItem.h"

namespace llvm {
class Module;
} // namespace llvm

namespace stone {

class CompilerContext final {
  Compiler &compiler;
  // TODO: prevScope,
  // CompilingScope scope;

  // InputFile inputFile
  llvm::Module *llvmModule = nullptr;

public:
  SafeList<CompilableItem> compilables;

private:
  CompilerContext(const CompilerContext &) = delete;
  CompilerContext(CompilerContext &&) = delete;
  CompilerContext &operator=(const CompilerContext &) = delete;
  CompilerContext &operator=(CompilerContext &&) = delete;

public:
  CompilerContext(Compiler &compiler) : compiler(compiler) {}
  ~CompilerContext();

public:
  Compiler &GetCompiler() { return compiler; }
  // CompilingScope &GetScope() { return scope; }
  void SetLLVMModule(llvm::Module *m) { llvmModule = m; }

  llvm::Module *GetLLVMModule() {
    assert(llvmModule && "No LLVM Module");
    return llvmModule;
  }

public:
  void AddCompilable(std::unique_ptr<CompilableItem> compilable) {
    compilables.Add(std::move(compilable));
  }
};

} // namespace stone
#endif
