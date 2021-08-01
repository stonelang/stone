#ifndef STONE_COMPILE_COMPILABLE_H
#define STONE_COMPILE_COMPILABLE_H

#include "stone/CodeGen/CodeGenListener.h"
#include "stone/Compile/CompilableFile.h"
#include "stone/Parse/SyntaxListener.h"
#include "stone/Semantics/TypeCheckerListener.h"

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

  virtual void NotifyListeners() = 0;

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
  void NotifyListeners() override;

protected:
  int DoCompileFile() override;
};

class TypeChecking final : public Compilable, public SyntaxListener {
  syn::SyntaxFile *syntaxFile = nullptr;

public:
  TypeChecking(Compiler &compiler);

public:
  void Finish() override;

public:
  void OnParseCompleted(syn::SyntaxFile *syntaxFile) override;

protected:
  void NotifyListeners() override;
  int DoCompileFile() override;
};

class EmittingIR final : public Compilable, public TypeCheckerListener {

  llvm::Module *llvmModule = nullptr;

public:
  EmittingIR(Compiler &compiler);

public:
  void Finish() override;
  void OnTypeCheckCompleted(syn::SyntaxFile *syntaxFile) override;
  void NotifyListeners() override;

protected:
  int DoCompileFile() override;
};

class EmittingObject final : public Compilable, public EmittingIRListener {

public:
  EmittingObject(Compiler &compiler);

public:
  void Finish() override;

  void OnEmitIRCompleted(llvm::Module *m) override;
  void NotifyListeners() override;

protected:
  int DoCompileFile() override;
};

class EmittingModule final : public Compilable, public EmittingIRListener {

public:
  EmittingModule(Compiler &compiler);

public:
  void OnEmitIRCompleted(llvm::Module *m) override;
  void Finish() override;
  void NotifyListeners() override;

protected:
  int DoCompileFile() override;
};

class EmittingBitCode final : public Compilable, public EmittingIRListener {
public:
  EmittingBitCode(Compiler &compiler);

public:
  void OnEmitIRCompleted(llvm::Module *m) override;
  void Finish() override;
  void NotifyListeners() override;

protected:
  int DoCompileFile() override;
};

class EmittingAssembly final : public Compilable, public EmittingIRListener {
public:
  EmittingAssembly(Compiler &compiler);

public:
  void OnEmitIRCompleted(llvm::Module *m) override;
  void Finish() override;
  void NotifyListeners() override;

protected:
  int DoCompileFile() override;
};

class EmittingLibrary final : public Compilable, public EmittingIRListener {

public:
  EmittingLibrary(Compiler &compiler);

public:
  void OnEmitIRCompleted(llvm::Module *m) override;
  void Finish() override;
  void NotifyListeners() override;

protected:
  int DoCompileFile() override;
};

} // namespace stone
#endif
