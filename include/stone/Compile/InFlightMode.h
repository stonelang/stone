#ifndef STONE_COMPILE_INFLIGHTMODE_H
#define STONE_COMPILE_INFLIGHTMODE_H

#include "stone/Compile/CompilableItem.h"

namespace stone {
namespace syn {
SyntaxFile;
}
class compiler;
class InFlightFile final {
public:
};
class InFlightMode {

  Compiler &compiler;
  InFlightFile inFlightFile;

public:
  InFlightMode(Compiler &compiler);

public:
  virtual int Execute() = 0;
};

class SyntaxInFlightMode : public InFlightMode {
  syn::SyntaxFile *syntaxFile;

public:
  SyntaxInFlightMode(Compiler &compiler);
private:
  syn::SyntaxFile *GetSyntaxFile() { return syntaxFile; }

public:
  virtual int Execute() = 0;
};

class ParseInFlightMode : public SyntaxInFlightMode {
public:
  ParseInFlightMode(Compiler &compiler);

public:
  int Execute() override;
};

class TypeCheckInFlightMode : public ParseInFlightMode {
public:
  TypeCheckInFlightMode(Compiler &compiler);

public:
  int Execute() override;
};

class EmitIRInFlightMode : public TypeCheckInFlightMode {
  llvm::Module *llvmModule;

public:
  EmitIRInFlightMode(Compiler &compiler);

public:
  llvm::Module *GetLLVMModule() { return llvmModule; }

public:
  int Execute() override;
};

class EmitModuleInFlightMode : public EmitIRInFlightMode {
public:
  EmitModuleInFlightMode(Compiler &compiler);

public:
  int Execute() override;
};

class EmitBitCodeInFlightMode : public EmitIRInFlightMode {
public:
  EmitBitCodeInFlightMode(Compiler &compiler);

public:
  int Execute() override;
};

class EmitObjectInFlightMode : public EmitIRInFlightMode {
public:
  EmitObjectInFlightMode(Compiler &compiler);

public:
  int Execute() override;
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
