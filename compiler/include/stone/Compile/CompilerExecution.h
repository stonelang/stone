#ifndef STONE_COMPILE_COMPILER_EXECUTION_H
#define STONE_COMPILE_COMPILER_EXECUTION_H

#include "stone/Basic/ColorStream.h"
#include "stone/Basic/Status.h"
#include "stone/Compile/CompilerObservation.h"
#include "stone/Compile/CompilerOptions.h"
#include "stone/Gen/IRGenInstance.h"

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

class CompilerExecution {

protected:
  Compiler &compiler;
  CompilerExecution *consumer = nullptr;

  llvm::sys::TimePoint<> startTime = std::chrono::system_clock::now();
  llvm::sys::TimePoint<> lastTime = llvm::sys::TimePoint<>::min();

public:
  CompilerExecution(Compiler &compiler);
  virtual ~CompilerExecution();

public:
  llvm::sys::TimePoint<> GetStartTime() const { return startTime; }
  llvm::sys::TimePoint<> GetLastTime() const { return lastTime; }

public:
  /// Setup the execution and execute any dependencies
  virtual Status SetupAction();

  /// Execut the action
  virtual Status ExecuteAction() = 0;

  /// Finish any post steps after execution
  virtual Status FinishAction();

  /// Every exeuction must have a self action
  virtual CompilerAction GetSelfAction() = 0;

  /// Check that the execution has an action
  bool HasSelfAction() { return CompilerOptions::IsAnyAction(GetSelfAction()); }

  /// Check the nice name of the current action
  llvm::StringRef GetSelfActionString() {
    return CompilerOptions::GetActionString(GetSelfAction());
  }

  /// The main input action from the user.
  CompilerAction GetMainAction();

  /// Check the nice name of the current action
  llvm::StringRef GetMainActionString() {
    return CompilerOptions::GetActionString(GetMainAction());
  }

  /// Determine if the main action and the self action is the same.
  bool IsMainAction() { return GetSelfAction() == GetMainAction(); }

  /// Get the dependency action
  virtual CompilerAction GetDepAction() { return CompilerAction::None; }

  /// Check that there exist a dependecy action
  bool HasDepAction() { return CompilerOptions::IsAnyAction(GetDepAction()); }

  /// Check the nice name of the current action
  llvm::StringRef GetDepActionString() {
    return CompilerOptions::GetActionString(GetDepAction());
  }

  /// Check that there exist a consumer
  bool HasConsumer() { return GetConsumer() != nullptr; }

  /// the the consumer
  void SetConsumer(CompilerExecution *inputConsumer) {
    consumer = inputConsumer;
  }
  /// Return the consumer
  CompilerExecution *GetConsumer();

  /// Make sure that we can notify the consumer
  bool ShouldNotifyConsumer() { return (!IsMainAction() && HasConsumer()); }

  /// A main action should never have a consumer
  void VerifyMainActionHasNoConsumer();

  /// Return the compiler
  Compiler &GetCompiler();

  virtual void Print(ColorStream &stream) const;

public:
  virtual void CompletedSourceFile(SourceFile& result);
  virtual void CompletedModuleDecl(ModuleDecl& moduleDecl);
  virtual void CompletedIRGeneration(llvm::Module *result);
};

class PrintHelpExecution final : public CompilerExecution {
public:
  PrintHelpExecution(Compiler &compiler);

public:
  Status ExecuteAction() override;
  CompilerAction GetSelfAction() override { return CompilerAction::PrintHelp; }
};

class PrintHelpHiddenExecution final : public CompilerExecution {
public:
  PrintHelpHiddenExecution(Compiler &compiler);

public:
  Status ExecuteAction() override;
  CompilerAction GetSelfAction() override {
    return CompilerAction::PrintHelpHidden;
  }
};

class PrintVersionExecution final : public CompilerExecution {

public:
  PrintVersionExecution(Compiler &compiler);

public:
  Status ExecuteAction() override;
  CompilerAction GetSelfAction() override {
    return CompilerAction::PrintVersion;
  }
};

class PrintFeatureExecution final : public CompilerExecution {
public:
  PrintFeatureExecution(Compiler &compiler);

public:
  Status ExecuteAction() override;
  CompilerAction GetSelfAction() override {
    return CompilerAction::PrintFeature;
  }
};

class ParseExecution final : public CompilerExecution {
public:
  ParseExecution(Compiler &compiler);

public:
  Status ExecuteAction() override;
  CompilerAction GetSelfAction() override { return CompilerAction::Parse; }

public:
};
class ResolveImportsExecution final : public CompilerExecution {
public:
  ResolveImportsExecution(Compiler &compiler);

public:
  Status ExecuteAction() override;
  CompilerAction GetDepAction() override { return CompilerAction::Parse; }
  CompilerAction GetSelfAction() override {
    return CompilerAction::ResolveImports;
  }

public:
  void CompletedSourceFile(SourceFile &result) override;
  void CompletedModuleDecl(ModuleDecl &result) override;
};

class PrintASTBeforeExecution final : public CompilerExecution {
public:
  PrintASTBeforeExecution(Compiler &compiler);

public:
  Status ExecuteAction() override;

public:
  CompilerAction GetSelfAction() override {
    return CompilerAction::PrintASTBefore;
  }
  CompilerAction GetDepAction() override { return CompilerAction::Parse; }

  void CompletedSourceFile(SourceFile &result) override;
  void CompletedModuleDecl(ModuleDecl &result) override;
};

class TypeCheckExecution final : public CompilerExecution {
public:
  TypeCheckExecution(Compiler &compiler);

public:
  Status ExecuteAction() override;

public:
  CompilerAction GetDepAction() override {
    return CompilerAction::ResolveImports;
  }
  CompilerAction GetSelfAction() override { return CompilerAction::TypeCheck; }

  void CompletedSourceFile(SourceFile &result) override;
  void CompletedModuleDecl(ModuleDecl &result) override;
};

class PrintASTAfterExecution final : public CompilerExecution {
public:
  PrintASTAfterExecution(Compiler &compiler);

public:
  Status ExecuteAction() override;

public:
  CompilerAction GetDepAction() override { return CompilerAction::TypeCheck; }
  CompilerAction GetSelfAction() override {
    return CompilerAction::PrintASTAfter;
  }

  void CompletedSourceFile(SourceFile &result) override;
  void CompletedModuleDecl(ModuleDecl &result) override;
};

// Generate IR, before optimization
class EmitIRBeforeExecution final : public CompilerExecution {
  // llvm::SmallVector<llvm::Module *> modules;
public:
  EmitIRBeforeExecution(Compiler &compiler);

public:
  Status ExecuteAction() override;
  Status FinishAction() override;
  CompilerAction GetDepAction() override { return CompilerAction::TypeCheck; }
  CompilerAction GetSelfAction() override {
    return CompilerAction::EmitIRBefore;
  }

public:
  void CompletedSourceFile(SourceFile &result) override;
  void CompletedModuleDecl(ModuleDecl &result) override;
};

// Generate IR, optimize ir, then print it.
class EmitIRAfterExecution final : public CompilerExecution {

public:
  EmitIRAfterExecution(Compiler &compiler);

public:
  Status ExecuteAction() override;
  Status FinishAction() override;
  CompilerAction GetDepAction() override {
    return CompilerAction::EmitIRBefore;
  }
  CompilerAction GetSelfAction() override {
    return CompilerAction::EmitIRAfter;
  }

public:
  void CompletedSourceFile(SourceFile &result) override;
  void CompletedModuleDecl(ModuleDecl &result) override;
};

// Generate IR, optimize ir, then print it.
class PrintIRExecution final : public CompilerExecution {

public:
  PrintIRExecution(Compiler &compiler);

public:
  Status ExecuteAction() override;
  Status FinishAction() override;

  CompilerAction GetDepAction() override { return CompilerAction::EmitIRAfter; }
  CompilerAction GetSelfAction() override { return CompilerAction::PrintIR; }
};

class EmitBitCodeExecution final : public CompilerExecution {
public:
  EmitBitCodeExecution(Compiler &compiler);

public:
  Status ExecuteAction() override;
  CompilerAction GetDepAction() override { return CompilerAction::EmitIRAfter; }
  CompilerAction GetSelfAction() override { return CompilerAction::EmitBC; }
};

class EmitModuleExecution final : public CompilerExecution {
public:
  EmitModuleExecution(Compiler &compiler);

public:
  Status ExecuteAction() override;

public:
  CompilerAction GetDepAction() override { return CompilerAction::EmitIRAfter; }
  CompilerAction GetSelfAction() override { return CompilerAction::EmitModule; }
};

class MergeModulesExecution final : public CompilerExecution {
public:
  MergeModulesExecution(Compiler &compiler);

public:
  Status ExecuteAction() override;

public:
  CompilerAction GetSelfAction() override {
    return CompilerAction::MergeModules;
  }
};

class EmitObjectExecution final : public CompilerExecution {
public:
  EmitObjectExecution(Compiler &compiler);

public:
  Status ExecuteAction() override;
  CompilerAction GetDepAction() override { return CompilerAction::EmitIRAfter; }
  CompilerAction GetSelfAction() override { return CompilerAction::EmitObject; }
  Status FinishAction() override;

  void CompletedIRGeneration(llvm::Module *result) override;
};

class EmitAssemblyExecution final : public CompilerExecution {
public:
  EmitAssemblyExecution(Compiler &compiler);

public:
  Status ExecuteAction() override;
  CompilerAction GetDepAction() override { return CompilerAction::EmitIRAfter; }
  CompilerAction GetSelfAction() override {
    return CompilerAction::EmitAssembly;
  }
  Status FinishAction() override;
  void CompletedIRGeneration(llvm::Module *result) override;
};

/// Completeds LLVM
Status CompileLLVM(Compiler &compiler);

/// Completeds LLVM
Status CompileAction(Compiler &compiler);

} // namespace stone

#endif
