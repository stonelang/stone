#ifndef STONE_COMPILE_COMPILEROBSERVATION_H
#define STONE_COMPILE_COMPILEROBSERVATION_H

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
  virtual void CompletedCommandLineParsing(Compiler &compiler);

  /// The compiler has been configured
  virtual void CompletedConfiguration(Compiler &compiler);

  /// Completed syntax analysis
  virtual void CompletedSyntaxAnalysis(Compiler &compiler);

  /// Completed syntax analysis
  virtual void CompletedSyntaxAnalysis(SourceFile &sourceFile);

  /// Completed syntax analysis
  virtual void CompletedSyntaxAnalysis(ModuleDecl &mod);

  /// Completed semantic analysis
  virtual void CompletedSemanticAnalysis(Compiler &compiler);

  /// Completed semantic analysis
  virtual void CompletedSemanticAnalysis(SourceFile &sourceFile);

  /// Completed semantic analysis
  virtual void CompletedSemanticAnalysis(ModuleDecl &mod);

  // Completed IR generation
  virtual void CompletedIRGeneration(Compiler &compiler);

  /// Some executions may require access to the results of ir generation.
  virtual void CompletedIRGeneration(llvm::Module *result);

  /// Some executions may require access to the results of ir generation.
  virtual void
  CompletedIRGeneration(llvm::ArrayRef<llvm::Module *, 8> &results);

  /// Completed native code generation
  virtual void CompletedNativeGeneration(Compiler &compiler);

  /// Callbacks into the parsing pipeline
  virtual CodeCompletionCallbacks *GetCodeCompletionCallbacks();
};

} // namespace stone

#endif