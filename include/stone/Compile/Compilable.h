#ifndef STONE_COMPILE_COMPILABLE_H
#define STONE_COMPILE_COMPILABLE_H

#include "stone/Basic/File.h"
#include "stone/Parse/SyntaxPipeline.h"

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

namespace mode {
class CompilableFile final {

  bool isPrimary;

  file::File *input = nullptr;
  /// The input, if it comes from a buffer rather than a file. This object
  /// does not own the buffer, and the caller is responsible for ensuring
  /// that it outlives any users.
  llvm::MemoryBuffer *buffer = nullptr;

public:
  CompilableFile() = default;

  CompilableFile(file::File *input, bool isPrimary)
      : CompilableFile(input, isPrimary, nullptr) {}

  /// Constructs an input file from the provided data.
  CompilableFile(file::File *input, bool isPrimary, llvm::MemoryBuffer *buffer)
      : input(input), isPrimary(isPrimary), buffer(buffer) {}

public:
  file::File &GetFile() {
    assert(input && "No File");
    return *input;
  }

  bool IsPrimary() { return isPrimary; }
  /// Retrieves the backing buffer for this input file, if any.
  llvm::MemoryBuffer *GetBuffer() { return buffer; }
};
} // namespace mode

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

class alignas(8) Compilable {

protected:
  Compiler &compiler;
  mode::CompilableFile input;

public:
  explicit Compilable(Compiler &compiler);
  virtual ~Compilable() { Finish(); }
  virtual void Finish();

protected:
  void SetCompilableFile(const mode::CompilableFile &input) {
    this->input = input;
  }

public:
  const mode::CompilableFile &GetCompilableFile() { return input; }
  Compiler &GetCompiler() { return compiler; }

  int CompileFile(const mode::CompilableFile &input);

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

class TypeChecking final : public Compilable {
  SyntaxParsing syntaxParsing;

public:
  TypeChecking(Compiler &compiler);

public:
  void Finish() override;
  syn::SyntaxFile *GetSyntaxFile() { return syntaxParsing.GetSyntaxFile(); }

protected:
  int DoCompileFile() override;
};

class OutputCompilable : public Compilable {
public:
  explicit OutputCompilable(Compiler &compiler) : Compilable(compiler) {}
};

class EmittingIR final : public OutputCompilable {

  llvm::Module *llvmModule;
  CodeGenPipeline *pipeline;
  TypeChecking typeChecking;

public:
  EmittingIR(Compiler &compiler);

public:
  llvm::Module *GetLLVMModule() { return llvmModule; }

public:
  void Finish() override;

protected:
  int DoCompileFile() override;
};

class EmittingObject final : public OutputCompilable {

  EmittingIR emittingIR;

public:
  EmittingObject(Compiler &compiler);

public:
  void Finish() override;

protected:
  int DoCompileFile() override;
};

class EmittingModule final : public OutputCompilable {

  EmittingIR emittingIR;

public:
  EmittingModule(Compiler &compiler);

public:
  void Finish() override;

protected:
  int DoCompileFile() override;
};

class EmittingBitCode final : public OutputCompilable {

  EmittingIR emittingIR;

public:
  EmittingBitCode(Compiler &compiler);

public:
  void Finish() override;

protected:
  int DoCompileFile() override;
};

class EmittingAssembly final : public OutputCompilable {

  EmittingIR emittingIR;

public:
  EmittingAssembly(Compiler &compiler);

public:
  void Finish() override;

protected:
  int DoCompileFile() override;
};

class EmittingLibrary final : public OutputCompilable {

  EmittingIR emittingIR;

public:
  EmittingLibrary(Compiler &compiler);

public:
  void Finish() override;

protected:
  int DoCompileFile() override;
};

} // namespace stone
#endif
