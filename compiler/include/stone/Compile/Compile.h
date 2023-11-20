#ifndef STONE_COMPILE_COMPILE_H
#define STONE_COMPILE_COMPILE_H

#include "llvm/ADT/ArrayRef.h"

namespace stone {

class Status;
class CodeGenContext;
class CompilerListener;

int Compile(llvm::ArrayRef<const char *> args, const char *arg0, void *mainAddr,
            CompilerListener *listener = nullptr);

class CompileStatus final {
  Status status;
public:
  enum Flags {
    ForSupport = 1 << 0,
    BeforeCodeAnalysis = 1 << 1,
    ForCodeAnalysis = 1 << 2,
    ForSyntaxAnalysis = 1 << 3,
    ForSemanticAnalysis = 1 << 4,
    AfterCodeAnalysis = 1 << 5,
    ForCodeGen = 1 << 6,
    ForGenIR = 1 << 7,
    ForGenMachine = 1 << 8,
  };

public:
  bool ShouldContinue() { return (!status.IsError() && !status.HasCompletion()); }

public:
  CompileStatus(Status &status) : status(status) {}

public:
  Status &GetStatus() { return status; }
};

/// Handles help, version, features
void CompileForSupport(Compiler &compiler, CompileStatus &status);

/// Handles PCH, ...
void CompileBeforeCodeAnalysis(Compiler &compiler, CompileStatus &status);

/// Handles parsings, import resolution, and type-checking
void CompileForCodeAnalysis(Compiler &compiler, CompileStatus &status);

/// Handles only syntax
void CompileForSyntaxAnalysis(Compiler &compiler, CompileStatus &status);

/// Handles semantics
void CompileForSemanticAnalysis(Compiler &compiler, CompileStatus &status);

/// Handles taskes before code generation
void CompileAfterSemanticAnalysis(Compiler &compiler, CompileStatus &status);

/// Handles code generating
void CompileForCodeGen(Compiler &compiler, CompileStatus &status);

/// Handles IR generation
void CompileForGenIR(Compiler &compiler, CompileStatus &status,
                     CodeGenContext &codeGenContext);

/// Handles machine code generation
void CompileForGenMachine(Compiler &compiler, CompileStatus &status,
                          CodeGenContext &codeGenContext);

} // namespace stone

#endif
