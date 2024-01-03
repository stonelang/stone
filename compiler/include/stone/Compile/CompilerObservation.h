#ifndef STONE_COMPILE_COMPILER_OBSERVATION_H
#define STONE_COMPILE_COMPILER_OBSERVATION_H

#include "llvm/ADT/ArrayRef.h"

namespace llvm {
class Module;
}
namespace stone {
class Compiler;
class SourceFile;
class ModuleDecl;
class CodeCompletionCallbacks;

class CompilerObservation {
public:
  CompilerObservation() = default;
  virtual ~CompilerObservation() = default;

public:
  /// The command line has been parsed.
  virtual void CompletedCommandLineParsing(Compiler &result);

  /// The compiler has been configured
  virtual void CompletedConfiguration(Compiler &result);

  /// Completed syntax analysis
  virtual void CompletedSyntaxAnalysis(Compiler &result);

  /// Completed syntax analysis
  virtual void CompletedSyntaxAnalysis(SourceFile &result);

  /// Completed syntax analysis
  virtual void CompletedSyntaxAnalysis(ModuleDecl &result);

  /// Completed semantic analysis
  virtual void CompletedSemanticAnalysis(Compiler &result);

  /// Completed semantic analysis
  virtual void CompletedSemanticAnalysis(SourceFile &result);

  /// Completed semantic analysis
  virtual void CompletedSemanticAnalysis(ModuleDecl &result);

  // Completed IR generation
  virtual void CompletedIRGeneration(Compiler &result);

  /// Some executions may require access to the results of ir generation.
  virtual void CompletedIRGeneration(llvm::Module *result);

  /// Some executions may require access to the results of ir generation.
  virtual void CompletedIRGeneration(llvm::ArrayRef<llvm::Module *> &results);

  /// Completed native code generation
  virtual void CompletedNativeGeneration(Compiler &result);

  /// Callbacks into the parsing pipeline
  virtual CodeCompletionCallbacks *GetCodeCompletionCallbacks();
};

} // namespace stone

#endif