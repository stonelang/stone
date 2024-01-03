#ifndef STONE_COMPILE_COMPILER_EXECUTION_H
#define STONE_COMPILE_COMPILER_EXECUTION_H

#include "stone/Basic/Color.h"
#include "stone/Basic/Status.h"
#include "stone/Compile/CompilerObservation.h"
#include "stone/Gen/IRGenModule.h"
#include "stone/Option/ActionKind.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/Support/Chrono.h"

namespace llvm {
Module;
}
namespace stone {

class Compiler;
class SourceFile;
class ModuleDecl;
class CompilerExecution;
class CodeCompletionCallbacks;

class CompilerExecution : public CompilerObservation {

protected:
  Compiler &compiler;
  CompilerExecution *consumer = nullptr;

  llvm::sys::TimePoint<> startTime;
  llvm::sys::TimePoint<> endTime = llvm::sys::TimePoint<>::min();

public:
  CompilerExecution(Compiler &compiler);
  virtual ~CompilerExecution();

protected:
  /// Every exeuction must have a self action
  virtual ActionKind GetSelfAction() { return ActionKind::None; }

  /// Check that there exist a dependecy action
  bool HasSelfAction() { return GetSelfAction() != ActionKind::None; }

public:
  /// Setup the execution and execute any dependencies
  virtual Status SetupAction();

  /// Execut the action
  virtual Status ExecuteAction() = 0;

  /// Finish any post steps after execution
  virtual Status FinishAction();

  /// The main input action from the user.
  ActionKind GetMainAction();

  /// Determine if the main action and the self action is the same.
  bool HasMainAction() { return GetSelfAction() == GetMainAction(); }

  /// Get the dependency action
  virtual ActionKind GetDepAction() { return ActionKind::None; }

  /// Check that there exist a dependecy action
  bool HasDepAction() { return GetDepAction() != ActionKind::None; }

  /// Check that there exist a consumer
  bool HasConsumer() { return GetConsumer() != nullptr; }

  /// the the consumer
  void SetConsumer(CompilerExecution *inputConsumer) {
    consumer = inputConsumer;
  }

  virtual CompilerExecution *GetConsumer();
  Compiler &GetCompiler();

public:
  /// The command line has been parsed.
  void CompletedCommandLineParsing(Compiler &result) override;

  /// The compiler has been configured
  void CompletedConfiguration(Compiler &result) override;

  /// Completed syntax analysis
  void CompletedSyntaxAnalysis(Compiler &result);

  /// Completed syntax analysis
  void CompletedSyntaxAnalysis(SourceFile &result) override;

  /// Completed syntax analysis
  void CompletedSyntaxAnalysis(ModuleDecl &result) override;

  /// Completed semantic analysis
  void CompletedSemanticAnalysis(Compiler &result) override;

  /// Completed semantic analysis
  void CompletedSemanticAnalysis(SourceFile &result) override;

  /// Completed semantic analysis
  void CompletedSemanticAnalysis(ModuleDecl &result) override;

  // Completed IR generation
  void CompletedIRGeneration(Compiler &result) override;

  /// Some executions may require access to the results of ir generation.
  void CompletedIRGeneration(llvm::Module *result) override;

  /// Some executions may require access to the results of ir generation.
  void CompletedIRGeneration(llvm::ArrayRef<llvm::Module *> &results) override;

  /// Callbacks into the parsing pipeline
  CodeCompletionCallbacks *GetCodeCompletionCallbacks() override;
};

class PrintHelpExecution final : public CompilerExecution {
public:
  PrintHelpExecution(Compiler &compiler);

public:
  Status ExecuteAction() override;
  ActionKind GetSelfAction() override { return ActionKind::PrintHelp; }
};

class PrintHelpHiddenExecution final : public CompilerExecution {
public:
  PrintHelpHiddenExecution(Compiler &compiler);

public:
  Status ExecuteAction() override;
  ActionKind GetSelfAction() override { return ActionKind::PrintHelpHidden; }
};

class PrintVersionExecution final : public CompilerExecution {

public:
  PrintVersionExecution(Compiler &compiler);

public:
  Status ExecuteAction() override;
  ActionKind GetSelfAction() override { return ActionKind::PrintVersion; }
};

class PrintFeatureExecution final : public CompilerExecution {
public:
  PrintFeatureExecution(Compiler &compiler);

public:
  Status ExecuteAction() override;
  ActionKind GetSelfAction() override { return ActionKind::PrintFeature; }
};

class ParseExecution final : public CompilerExecution {
public:
  ParseExecution(Compiler &compiler);

public:
  Status ExecuteAction() override;
  ActionKind GetSelfAction() override { return ActionKind::Parse; }

public:
};
class ImportResolutionExecution final : public CompilerExecution {
public:
  ImportResolutionExecution(Compiler &compiler);

public:
  Status ExecuteAction() override;
  ActionKind GetDepAction() override { return ActionKind::Parse; }
  ActionKind GetSelfAction() override { return ActionKind::ResolveImports; }

public:
  void CompletedSyntaxAnalysis(SourceFile &result) override;
  void CompletedSyntaxAnalysis(ModuleDecl &result) override;
};

class DumpASTExecution final : public CompilerExecution {
public:
  DumpASTExecution(Compiler &compiler);

public:
  Status ExecuteAction() override;

public:
  ActionKind GetSelfAction() override { return ActionKind::DumpAST; }
  ActionKind GetDepAction() override { return ActionKind::Parse; }

  // void CompletedSyntaxAnalysis(SourceFile *result) override;
};

class TypeCheckExecution final : public CompilerExecution {
public:
  TypeCheckExecution(Compiler &compiler);

public:
  Status ExecuteAction() override;

public:
  ActionKind GetDepAction() override { return ActionKind::ResolveImports; }
  ActionKind GetSelfAction() override { return ActionKind::TypeCheck; }

  void CompletedSyntaxAnalysis(SourceFile &result) override;
  void CompletedSyntaxAnalysis(ModuleDecl &result) override;

  virtual CompilerExecution *GetConsumer() override;
};

class PrintASTExecution final : public CompilerExecution {
public:
  PrintASTExecution(Compiler &compiler);

public:
  Status ExecuteAction() override;

public:
  ActionKind GetDepAction() override { return ActionKind::TypeCheck; }
  ActionKind GetSelfAction() override { return ActionKind::PrintAST; }

  void CompletedSyntaxAnalysis(SourceFile &result) override;
  void CompletedSyntaxAnalysis(ModuleDecl &result) override;
};

// Generate IR, before optimization
class EmitIRBeforeExecution final : public CompilerExecution {

public:
  EmitIRBeforeExecution(Compiler &compiler);

public:
  Status ExecuteAction() override;
  Status FinishAction() override;
  ActionKind GetDepAction() override { return ActionKind::TypeCheck; }
  ActionKind GetSelfAction() override { return ActionKind::EmitIRBefore; }

public:
  void CompletedSemanticAnalysis(SourceFile &result) override;
  void CompletedSemanticAnalysis(ModuleDecl &result) override;
};

// Generate IR, optimize ir, then print it.
class EmitIRAfterExecution final : public CompilerExecution {

public:
  EmitIRAfterExecution(Compiler &compiler);

public:
  Status ExecuteAction() override;
  Status FinishAction() override;
  ActionKind GetDepAction() override { return ActionKind::EmitIRBefore; }
  ActionKind GetSelfAction() override { return ActionKind::EmitIRAfter; }

public:
  void CompletedSemanticAnalysis(SourceFile &result) override;
  void CompletedSemanticAnalysis(ModuleDecl &result) override;
};

class EmitBitCodeExecution final : public CompilerExecution {
public:
  EmitBitCodeExecution(Compiler &compiler);

public:
  Status ExecuteAction() override;
  ActionKind GetDepAction() override { return ActionKind::EmitIRAfter; }
  ActionKind GetSelfAction() override { return ActionKind::EmitBC; }
};

class EmitModuleExecution final : public CompilerExecution {
public:
  EmitModuleExecution(Compiler &compiler);

public:
  Status ExecuteAction() override;

public:
  ActionKind GetDepAction() override { return ActionKind::EmitIRAfter; }
  ActionKind GetSelfAction() override { return ActionKind::EmitModule; }
};

// class EmitNativeExecution : public CompilerExecution {

//   /// NativeCodeGen
// public:
//   EmitNativeExecution(Compiler &compiler, ActionKind currentAction);

// public:
//   Status ExecuteAction() override;

//   ActionKind GetDepAction() override { return ActionKind::EmitIRAfter; }

//   CompilerExecution *GetConsumer() override;
//   void CompletedIRGeneration(llvm::ArrayRef<IRGenResult *, 8> results)
//   override;
// };

class EmitObjectExecution : public CompilerExecution {
public:
  EmitObjectExecution(Compiler &compiler);

public:
  Status ExecuteAction() override;
  ActionKind GetDepAction() override { return ActionKind::EmitIRAfter; }
  ActionKind GetSelfAction() override { return ActionKind::EmitObject; }
  Status FinishAction() override;

  void CompletedIRGeneration(llvm::Module *result) override;
  void CompletedIRGeneration(llvm::ArrayRef<llvm::Module *> &results) override;
};

class EmitAssemblyExecution : public CompilerExecution {
public:
  EmitAssemblyExecution(Compiler &compiler);

public:
  Status ExecuteAction() override;
  ActionKind GetDepAction() override { return ActionKind::EmitIRAfter; }
  ActionKind GetSelfAction() override { return ActionKind::EmitAssembly; }
  Status FinishAction() override;

  void CompletedIRGeneration(llvm::Module *result) override;
  void CompletedIRGeneration(llvm::ArrayRef<llvm::Module *> &results) override;
};
} // namespace stone

#endif
