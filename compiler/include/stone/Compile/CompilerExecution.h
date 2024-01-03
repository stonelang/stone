#ifndef STONE_COMPILE_COMPILER_EXECUTION_H
#define STONE_COMPILE_COMPILER_EXECUTION_H

#include "stone/Basic/Color.h"
#include "stone/Basic/Status.h"
#include "stone/Compile/CompilerObservation.h"
#include "stone/Gen/IRGenModule.h"
#include "stone/Option/ActionKind.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/Support/Chrono.h"

namespace stone {

class Compiler;
class SourceFile;
class CompilerExecution;

class CompilerExecution {

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
  Compiler &GetCompiler() { return compiler; }

public:
  // void CompletedCommandLineParsing(Compiler &compiler) override;

  // /// Completed syntax analysis
  // virtual void CompletedSyntaxAnalysis(SourceFile &sourceFile) override;

  // /// Completed syntax analysis
  // virtual void CompletedSyntaxAnalysis(ModuleDecl &mod) override;

  // /// Completed semantic analysis
  // virtual void CompletedSemanticAnalysis(SourceFile &sourceFile) override;

  // /// Completed semantic analysis
  // virtual void CompletedSemanticAnalysis(ModuleDecl &mod) override;

  // /// Some executions may require access to the results of ir generation.
  // virtual void CompletedIRGeneration(llvm::ArrayRef<IRGenResult *, 8>
  // &results);

  // /// Some executions may require access to the results of ir generation.
  // virtual void CompletedIRGeneration(llvm::Module *result);

  // /// Some executions may require access to the results of ir generation.
  // virtual void
  // CompletedIRGeneration(llvm::ArrayRef<llvm::Module *, 8> &results);
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
  virtual CompilerExecution *GetConsumer() override;

  // void CompletedSyntaxAnalysis(SourceFile *result) override;
};

class DumpASTExecution final : public CompilerExecution {
public:
  DumpASTExecution(Compiler &compiler);

public:
  Status ExecuteAction() override;

public:
  ActionKind GetSelfAction() override { return ActionKind::DumpAST; }
  ActionKind GetDepAction() override { return ActionKind::Parse; }
  virtual CompilerExecution *GetConsumer() override;

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
  virtual CompilerExecution *GetConsumer() override;

  // void CompletedSyntaxAnalysis(SourceFile *result) override;
  // void CompletedSyntaxAnalysis(ModuleDecl *result) override;
};

class PrintASTExecution final : public CompilerExecution {
public:
  PrintASTExecution(Compiler &compiler);

public:
  Status ExecuteAction() override;

public:
  ActionKind GetDepAction() override { return ActionKind::TypeCheck; }
  ActionKind GetSelfAction() override { return ActionKind::PrintAST; }
  virtual CompilerExecution *GetConsumer() override;
};

// Generate IR, before optimization
class GenerateIRExecution final : public CompilerExecution {

public:
  GenerateIRExecution(Compiler &compiler);

public:
  Status ExecuteAction() override;
  ActionKind GetDepAction() override { return ActionKind::TypeCheck; }
  ActionKind GetSelfAction() override { return ActionKind::EmitIRBefore; }
  virtual CompilerExecution *GetConsumer() override;
  Status FinishAction() override;
};

// Generate IR, optimize ir, then print it.
class OptimizeIRExecution final : public CompilerExecution {

  // IRCodeOptimizer;
public:
  OptimizeIRExecution(Compiler &compiler);

public:
  Status ExecuteAction() override;
  ActionKind GetDepAction() override { return ActionKind::EmitIRBefore; }
  ActionKind GetSelfAction() override { return ActionKind::EmitIRAfter; }
  virtual CompilerExecution *GetConsumer() override;
};

class EmitBitCodeExecution final : public CompilerExecution {
public:
  EmitBitCodeExecution(Compiler &compiler);

public:
  Status ExecuteAction() override;
  ActionKind GetDepAction() override { return ActionKind::EmitIRAfter; }
  ActionKind GetSelfAction() override { return ActionKind::EmitBC; }
  virtual CompilerExecution *GetConsumer() override;
};

class EmitModuleExecution final : public CompilerExecution {
public:
  EmitModuleExecution(Compiler &compiler);

public:
  Status ExecuteAction() override;

public:
  ActionKind GetDepAction() override { return ActionKind::EmitIRAfter; }
  ActionKind GetSelfAction() override { return ActionKind::EmitModule; }
  virtual CompilerExecution *GetConsumer() override;
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
  CompilerExecution *GetConsumer() override;

  // void CompletedIRGeneration(llvm::ArrayRef<IRGenResult *, 8> results)
  // override;
};

class EmitAssemblyExecution : public CompilerExecution {
public:
  EmitAssemblyExecution(Compiler &compiler);

public:
  Status ExecuteAction() override;
  ActionKind GetDepAction() override { return ActionKind::EmitIRAfter; }
  ActionKind GetSelfAction() override { return ActionKind::EmitAssembly; }
  CompilerExecution *GetConsumer() override;

  // void CompletedIRGeneration(llvm::ArrayRef<IRGenResult *, 8> results)
  // override;
};

// class FallbackExecution final : public CompilerExecution {

// public:
//   FallbackExecution(Compiler &compiler);

// public:
//   Status ExecuteAction() override;
// };

} // namespace stone

#endif
