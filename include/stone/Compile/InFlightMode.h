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
  file::File &input;
  bool isPrimary;
  /// The input, if it comes from a buffer rather than a file. This object
  /// does not own the buffer, and the caller is responsible for ensuring
  /// that it outlives any users.
  llvm::MemoryBuffer *buffer = nullptr;

public:
  InFlightInputFile(file::File &input, bool isPrimary)
      : InFlightInputFile(input, isPrimary, nullptr) {}

  /// Constructs an input file from the provided data.
  InFlightInputFile(file::File &input, bool isPrimary,
                    llvm::MemoryBuffer *buffer)
      : input(input), isPrimary(isPrimary), buffer(buffer) {}

public:
  file::File &GetFile() { return input; }

  bool IsPrimary() { return isPrimary; }
  /// Retrieves the backing buffer for this input file, if any.
  llvm::MemoryBuffer *GetBuffer() { return buffer; }
};
class InFlightMode {

protected:
  Compiler &compiler;
  // TODO: May want to use PumbPtr
  std::unique_ptr<InFlightInputFile> inFlightInputFile;

public:
  InFlightMode(Compiler &compiler);
  virtual ~InFlightMode() { Finish(); }

public:
  void SetInFlightFile(std::unique_ptr<InFlightInputFile> input) {
    // inFlightInputFile(std::move(input));
  }
  InFlightInputFile &GetInFlightInputFile() { return *inFlightInputFile.get(); }
  Compiler &GetCompiler() { return compiler; }

public:
  virtual int Execute() = 0;
  virtual void Finish() = 0;
};

class SyntaxInFlightMode : public InFlightMode {

  syn::SyntaxFile *syntaxFile;
  /// Hmm... think about
  friend class ParseInFlightMode;
  friend class TypeCheckInFlightMode;
  friend class EmitIRInFlightMode;

public:
  SyntaxInFlightMode(Compiler &compiler);

public:
  virtual int Execute() = 0;
  virtual void Finish() = 0;
};

class ParseInFlightMode : public SyntaxInFlightMode {
public:
  ParseInFlightMode(Compiler &compiler);

public:
  int Execute() override;
  void Finish() override;
};

class TypeCheckInFlightMode : public ParseInFlightMode {

public:
  TypeCheckInFlightMode(Compiler &compiler);

public:
  int Execute() override;
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
  int Execute() override;
  void Finish() override;
};

class EmitModuleInFlightMode : public EmitIRInFlightMode {
public:
  EmitModuleInFlightMode(Compiler &compiler);

public:
  int Execute() override;
  void Finish() override;
};

class EmitBitCodeInFlightMode : public EmitIRInFlightMode {
public:
  EmitBitCodeInFlightMode(Compiler &compiler);

public:
  int Execute() override;
  void Finish() override;
};

class EmitObjectInFlightMode : public EmitIRInFlightMode {
public:
  EmitObjectInFlightMode(Compiler &compiler);

public:
  int Execute() override;
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
