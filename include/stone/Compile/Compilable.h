#ifndef STONE_COMPILE_COMPILABLE_H
#define STONE_COMPILE_COMPILABLE_H

#include "stone/CodeGen/CodeGenPipelineListener.h"
#include "stone/Compile/CompilableFile.h"
#include "stone/Parse/SyntaxPipelineListener.h"
#include "stone/Semantics/TypeCheckerPipelineListener.h"

#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/Path.h"

#include <string>

namespace llvm {
class Module;
} // namespace llvm

namespace stone {
class CodeGenPipeline;
namespace codegen {
class CodeGenOptions;
}
namespace syn {
class SyntaxFile;
}
class Compiler;

class CompilableScope final {
public:
  CompilableScope(const CompilableScope &) = delete;
  CompilableScope(CompilableScope &&) = delete;
  CompilableScope &operator=(const CompilableScope &) = delete;
  CompilableScope &operator=(CompilableScope &&) = delete;

public:
  CompilableScope() {}
  ~CompilableScope() {}

public:
  void Enter();
  void Exit();
};

/// TODO: CompilableMode
class alignas(8) Compilable {

protected:
  Compiler &compiler;
  CompilableFile input;

  SafeList<PipelineListener> listeners;

public:
  explicit Compilable(Compiler &compiler);
  virtual ~Compilable() { Finish(); }
  virtual void Finish();

protected:
  void SetCompilableFile(const CompilableFile &input) { this->input = input; }

public:
  const CompilableFile &GetCompilableFile() { return input; }
  Compiler &GetCompiler() { return compiler; }

  void AddListener(std::unique_ptr<PipelineListener> listener) {
    listeners.Add(std::move(listener));
  }
  int CompileFile(const CompilableFile &input);

protected:
  virtual int DoCompileFile() = 0;
};

class SyntaxParsing final : public Compilable {

protected:
  syn::SyntaxFile *syntaxFile;

public:
  SyntaxParsing(Compiler &compiler);

public:
  void Finish() override;
  syn::SyntaxFile *GetSyntaxFile() { return syntaxFile; }

protected:
  int DoCompileFile() override;
};

class TypeChecking final : public Compilable, public SyntaxPipelineListener {
  SyntaxParsing syntaxParsing;

public:
  TypeChecking(Compiler &compiler);

public:
  void Finish() override;
  syn::SyntaxFile *GetSyntaxFile() { return syntaxParsing.GetSyntaxFile(); }

public:
  void OnSyntaxFileParsed(syn::SyntaxFile *syntaxFile) override;

protected:
  int DoCompileFile() override;
};

class OutputCompilable : public Compilable {
public:
  explicit OutputCompilable(Compiler &compiler) : Compilable(compiler) {}
};

class EmittingIR final : public OutputCompilable,
                         public TypeCheckerPipelineListener {

  llvm::Module *llvmModule;
  TypeChecking typeChecking;

public:
  EmittingIR(Compiler &compiler);

public:
  llvm::Module *GetLLVMModule() { return llvmModule; }

public:
  void Finish() override;

  void OnSyntaxFileTypeChecked(syn::SyntaxFile *syntaxFile) override;
  void OnModuleTypeChecked(syn::SyntaxFile *syntaxFile) override;

protected:
  int DoCompileFile() override;
};

class EmittingObject final : public OutputCompilable,
                             public EmittingIRPipelineListener {

  EmittingIR emittingIR;

public:
  EmittingObject(Compiler &compiler);

public:
  void Finish() override;

  void OnIREmitted(llvm::Module *m) override;

protected:
  int DoCompileFile() override;
};

class EmittingModule final : public OutputCompilable,
                             public EmittingIRPipelineListener {

  EmittingIR emittingIR;

public:
  EmittingModule(Compiler &compiler);

public:
  void OnIREmitted(llvm::Module *m) override;
  void Finish() override;

protected:
  int DoCompileFile() override;
};

class EmittingBitCode final : public OutputCompilable,
                              public EmittingIRPipelineListener {

  EmittingIR emittingIR;

public:
  EmittingBitCode(Compiler &compiler);

public:
  void OnIREmitted(llvm::Module *m) override;
  void Finish() override;

protected:
  int DoCompileFile() override;
};

class EmittingAssembly final : public OutputCompilable,
                               public EmittingIRPipelineListener {

  EmittingIR emittingIR;

public:
  EmittingAssembly(Compiler &compiler);

public:
  void OnIREmitted(llvm::Module *m) override;
  void Finish() override;

protected:
  int DoCompileFile() override;
};

class EmittingLibrary final : public OutputCompilable,
                              public EmittingIRPipelineListener {

  EmittingIR emittingIR;

public:
  EmittingLibrary(Compiler &compiler);

public:
  void OnIREmitted(llvm::Module *m) override;
  void Finish() override;

protected:
  int DoCompileFile() override;
};

} // namespace stone
#endif
