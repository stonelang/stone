#ifndef STONE_COMPILE_INFLIGHTMODE_H
#define STONE_COMPILE_INFLIGHTMODE_H

#include "stone/Basic/File.h"

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

class InFlightCompile {
public:
  InFlightCompile() {}
};

class InFlightInputFile final {

  bool isPrimary;

  file::File *input = nullptr;
  /// The input, if it comes from a buffer rather than a file. This object
  /// does not own the buffer, and the caller is responsible for ensuring
  /// that it outlives any users.
  llvm::MemoryBuffer *buffer = nullptr;

public:
  InFlightInputFile() = default;

  InFlightInputFile(file::File *input, bool isPrimary)
      : InFlightInputFile(input, isPrimary, nullptr) {}

  /// Constructs an input file from the provided data.
  InFlightInputFile(file::File *input, bool isPrimary,
                    llvm::MemoryBuffer *buffer)
      : input(input), isPrimary(isPrimary), buffer(buffer) {}

public:
  bool IsPrimary() { return isPrimary; }

  file::File &GetFile() {
    assert(input && "No File associated with the InFlightInputFile");
    return *input;
  }
  /// Retrieves the backing buffer for this input file, if any.
  llvm::MemoryBuffer &GetBuffer() {
    assert(buffer && "No MemboryBuffer associated with the InFlightInputFile");
    return *buffer;
  }
};
class InFlightMode {

protected:
  Compiler &compiler;

  InFlightInputFile inFlightInputFile;

public:
  explicit InFlightMode(Compiler &compiler);
  virtual ~InFlightMode() { Finish(); }

protected:
  void SetInFlightInputFile(const InFlightInputFile &inFlightInputFile) {
    this->inFlightInputFile = inFlightInputFile;
  }

public:
  InFlightInputFile &GetInFlightInputFile() { return inFlightInputFile; }
  Compiler &GetCompiler() { return compiler; }

public:
  virtual int Execute(const InFlightInputFile &inFlightInputFile);
  virtual void Finish() = 0;

protected:
  int Execute() = 0;
};

class SyntaxInFlightMode : public InFlightMode {

  syn::SyntaxFile *syntaxFile;
  friend class TypeCheckInFlightMode;
  friend class EmitIRInFlightMode;

public:
  SyntaxInFlightMode(Compiler &compiler);

public:
  int Execute(const InFlightInputFile &inFlightInputFile) override;
  void Finish() override;
};

class TypeCheckInFlightMode : public SyntaxInFlightMode {

public:
  TypeCheckInFlightMode(Compiler &compiler);

public:
  int Execute(const InFlightInputFile &inFlightInputFile) override;
  void Finish() override;
};

class EmitIRInFlightMode : public TypeCheckInFlightMode {

  llvm::Module *llvmModule;
  CodeGenPipeline *pipeline;

  friend class EmitModuleInFlightMode;
  friend class EmitObjectInFlightMode;
  friend class EmitBitCodeInFlightMode;

public:
  EmitIRInFlightMode(Compiler &compiler);

public:
  llvm::Module *GetLLVMModule() { return llvmModule; }

public:
  int Execute(const InFlightInputFile &inFlightInputFile) override;
  void Finish() override;
};

class EmitModuleInFlightMode : public EmitIRInFlightMode {
public:
  EmitModuleInFlightMode(Compiler &compiler);

public:
  int Execute(const InFlightInputFile &inFlightInputFile) override;
  void Finish() override;
};

class EmitBitCodeInFlightMode : public EmitIRInFlightMode {
public:
  EmitBitCodeInFlightMode(Compiler &compiler);

public:
  int Execute(const InFlightInputFile &inFlightInputFile) override;
  void Finish() override;
};

class EmitObjectInFlightMode : public EmitIRInFlightMode {
public:
  EmitObjectInFlightMode(Compiler &compiler);

public:
  int Execute(const InFlightInputFile &inFlightInputFile) override;
  void Finish() override;
};

// class EmitAssemblyInFlightMode : public EmitIRInFlightMode {
// public:
//   EmitAssemblyInFlightMode(Compiler &compiler);

// public:
//   int Execute() override;
// };

// class EmitLibraryInFlightMode : public EmitIRInFlightMode {
// public:
//   EmitLibraryInFlightMode(Compiler &compiler);

// public:
//   int Execute() override;
// };

} // namespace stone
#endif
