#ifndef STONE_COMPILE_COMPILE_H
#define STONE_COMPILE_COMPILE_H

#include "stone/Basic/Status.h"
#include "stone/IDE.h"

#include "llvm/ADT/ArrayRef.h"

namespace stone {
class Compiler;
class CompilerListener;

class CompilerObservation {
public:
  CompilerObservation() = default;
  virtual ~CompilerObservation() = default;

public:
  /// The command line has been parsed.
  virtual void CompletedCommandLineParsing(Compiler &compiler);

  /// The compiler has been configured
  virtual void CompletedConfiguration(Compiler &compiler);

  /// Completed syntax analysis
  virtual void CompletedSyntaxAnalysis(Compiler &compiler);

  /// Completed semantic analysis
  virtual void CompletedSemanticAnalysis(Compiler &compiler);

  // Completed IR generation
  virtual void CompletedIRGeneration(Compiler &compiler);

  /// Completed native code generation
  virtual void CompletedNativeGeneration(Compiler &compiler);

public:
  /// Callbacks into the parsing pipeline
  virtual CodeCompletionCallbacks *GetCodeCompletionCallbacks();
};

int Compile(llvm::ArrayRef<const char *> args, const char *arg0, void *mainAddr,
            CompilerListener *listener = nullptr);

/// Handles LLVM
Status CompileForLLVMIR(Compiler &compiler);

/// Handles valid actions
Status CompileForAction(Compiler &compiler);

/// Handles help, version, features
Status CompileForSupport(Compiler &compiler);

/// Handles only syntax
Status CompileForSyntaxAnalysis(Compiler &compiler);

/// Handles only syntax
Status CompileForParse(Compiler &compiler);

/// Handles only syntax
Status CompileForParseAndImportResolution(Compiler &compiler);

/// Handles semantics
Status CompileForTypeChecking(Compiler &compiler);

/// Handles tasks after
Status CompileAfterSemanticAnalysis(Compiler &compiler);

/// Handles code generating
Status CompileForEmitCode(Compiler &compiler);

/// Handles IR generation
// void CompileForGenIR(Compiler &compiler, CompilingSession &session,
//                      CodeGenContext &codeGenContext, ModuleOrSourceFile file,
//                                   const PrimarySpecificPaths &psps);

// /// Handles machine code generation
// void CompileForGenNative(Compiler &compiler, CompilingSession &session,
//                          CodeGenContext &codeGenContext);

} // namespace stone

#endif
