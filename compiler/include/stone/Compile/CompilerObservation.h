#ifndef STONE_COMPILE_COMPILER_OBSERVATION_H
#define STONE_COMPILE_COMPILER_OBSERVATION_H

#include "llvm/ADT/ArrayRef.h"
#include "llvm/IR/Module.h"

namespace stone {

class Compiler;
class SourceFile;
class ModuleDecl;
class CodeCompletionCallbacks;

class CompilerObservation {
public:
  CompilerObservation() {}
  virtual ~CompilerObservation() = default;

public:
  /// The command line has been parsed.
  virtual void CompletedCommandLineParsing(CompilerInstance &result) {}

  /// The compiler has been configured
  virtual void CompletedConfiguration(CompilerInstance &result) {}

  /// Completed syntax analysis
  virtual void CompletedSyntaxAnalysis(SourceFile &result) {}

  /// Completed syntax analysis
  virtual void CompletedSyntaxAnalysis(ModuleDecl &result) {}

  /// Completed syntax analysis
  virtual void CompletedSyntaxAnalysis(CompilerInstance &result) {}

  /// Completed semantic analysis
  virtual void CompletedSemanticAnalysis(SourceFile &result) {}

  /// Completed semantic analysis
  virtual void CompletedSemanticAnalysis(ModuleDecl &result) {}

  /// Completed semantic analysis
  virtual void CompletedSemanticAnalysis(CompilerInstance &result) {}

  /// Some executions may require access to the results of ir generation.
  virtual void CompletedIRGeneration(llvm::Module *result) {}

  /// Some executions may require access to the results of ir generation.
  virtual void CompletedIRGeneration(llvm::ArrayRef<llvm::Module *> &results) {}

  // Completed IR generation
  virtual void CompletedIRGeneration(CompilerInstance &result) {}

  /// Callbacks into the parsing pipeline
  virtual CodeCompletionCallbacks *GetCodeCompletionCallbacks() {}
};

} // namespace stone

#endif