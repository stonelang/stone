#ifndef STONE_COMPILE_COMPILE_H
#define STONE_COMPILE_COMPILE_H

#include "stone/Basic/Status.h"

#include "llvm/ADT/ArrayRef.h"

namespace stone {

class Status;
class Compiler;
class CodeGenContext;
class CompilerListener;

int Compile(llvm::ArrayRef<const char *> args, const char *arg0, void *mainAddr,
            CompilerListener *listener = nullptr);

class CompileSession final {
  Status status;
  Compiler &compiler;

public:
  enum Kind {
    ForSupport = 1 << 0,
    ForLLVMIR = 1 << 1,
    ForCodeAnalysis = 1 << 2,
    ForCodeGeneration = 1 << 3,
  };

public:
  bool ShouldContinue() {
    return (!status.IsError() && !status.HasCompletion());
  }

public:
  CompileSession(Compiler &compiler) : compiler(compiler) {}
  ~CompileSession() {}

  // public:
  //   CompileSession::Kind GetKindForAction(ActionKind kind);

public:
  bool IsForSupport();
  bool IsForLLVMIR();

public:
  Status &GetStatus() { return status; }
};

/// Handles LLVM
void Compile(Compiler &compiler);

/// Handles LLVM
void CompileForLLVMIR(Compiler &compiler, CompileSession &session);

/// Handles valid actions
void CompileForAction(Compiler &compiler, CompileSession &session);

/// Handles help, version, features
void CompileForSupport(Compiler &compiler, CompileSession &session);

/// Handles parsings, import resolution, and type-checking
void CompileForCodeAnalysis(Compiler &compiler, CompileSession &session,
                            std::function<void(Compiler &, CompileSession &)>);

/// Handles only syntax
void CompileForSyntaxAnalysis(Compiler &compiler, CompileSession &session);

/// Handles only syntax
void CompileForParse(Compiler &compiler, CompileSession &session);

/// Handles only syntax
void CompileForParseAndImportResolution(Compiler &compiler,
                                        CompileSession &session);
/// Handles semantics
void CompileForSemanticAnalysis(Compiler &compiler, CompileSession &session);

/// Handles semantics
void CompileForTypeChecking(Compiler &compiler, CompileSession &session);

// /// Handles tasks after
// void CompileAfterSemanticAnalysis(Compiler &compiler,
//                                   CompilingSession &session);

/// Handles code generating
void CompileForCodeGen(Compiler &compiler, CompileSession &session);

/// Handles IR generation
// void CompileForGenIR(Compiler &compiler, CompilingSession &session,
//                      CodeGenContext &codeGenContext, ModuleOrSourceFile file,
//                                   const PrimarySpecificPaths &psps);

// /// Handles machine code generation
// void CompileForGenNative(Compiler &compiler, CompilingSession &session,
//                          CodeGenContext &codeGenContext);

} // namespace stone

#endif
