#ifndef STONE_COMPILE_COMPILERACTION_H
#define STONE_COMPILE_COMPILERACTION_H

#include "stone/Compile/CompilerOptions.h"

namespace stone {

class ModuleDecl;
class CompilerAction;
class CodeGenResult;
class CompilerInstance;
class CompilerObservation;

class CompilerAction {
protected:
  CompilerInstance &instance;
  CompilerAction *consumer = nullptr;

public:
  /// Setup the execution and execute any dependencies
  virtual bool SetupAction();

  /// Execut the action
  virtual bool ExecuteAction() = 0;

  /// Finish any post steps after execution
  virtual bool FinishAction() {}

  /// Every exeuction must have a self action
  virtual CompilerActionKind GetSelfActionKind() const = 0;

  /// Check that the execution has an action
  bool HasSelfActionKind() {
    return CompilerOptions::IsAnyAction(GetSelfActionKind());
  }

  /// Check the nice name of the current action
  llvm::StringRef GetSelfActionKindString() {
    return CompilerOptions::GetActionString(GetSelfActionKind());
  }

  /// The main input action from the user.
  CompilerActionKind GetPrimaryActionKind();

  /// Check the nice name of the current action
  llvm::StringRef GetPrimaryActionKindString() {
    return CompilerOptions::GetActionString(GetPrimaryActionKind());
  }

  /// Determine if the main action and the self action is the same.
  bool IsPrimaryActionKind() {
    return GetSelfActionKind() == GetPrimaryActionKind();
  }

  /// Check that there exist a dependecy action
  bool HasDepActionKind() {
    return CompilerOptions::IsAnyAction(GetDepActionKind());
  }

  /// Get the dependency action
  virtual CompilerActionKind GetDepActionKind() const {
    return CompilerActionKind::None;
  }
  /// Check the nice name of the current action
  llvm::StringRef GetDepActionKindString() {
    return CompilerOptions::GetActionString(GetDepActionKind());
  }
  /// Update the callee
  virtual void DepCompleted(CompilerAction *dep) {}

  /// Check that there exist a consumer
  bool HasConsumer() { return GetConsumer() != nullptr; }

  /// the the consumer
  void SetConsumer(CompilerAction *compilerAction) {
    consumer = compilerAction;
  }

  /// Return the consumer
  CompilerAction *GetConsumer() { return consumer; }

  /// Return the compiler
  CompilerInstance &GetCompilerInstance();

public:
  CompilerAction(CompilerInstance &instance) : instance(instance) {}
  virtual ~CompilerAction() {}

public:
  bool IsPrintHelpAction() const {
    return GetSelfActionKind() == CompilerActionKind::PrintHelp;
  }
  bool IsPrintHelpHiddenAction() const {
    return GetSelfActionKind() == CompilerActionKind::PrintHelpHidden;
  }
  bool IsPrintVersionAction() const {
    return GetSelfActionKind() == CompilerActionKind::PrintVersion;
  }
  bool IsPrintFeatureAction() const {
    return GetSelfActionKind() == CompilerActionKind::PrintFeature;
  }
  bool IsParseAction() const {
    return GetSelfActionKind() == CompilerActionKind::Parse;
  }
  bool IsEmitParseAction() const {
    return GetSelfActionKind() == CompilerActionKind::EmitParse;
  }
  bool IsResolveImportsAction() const {
    return GetSelfActionKind() == CompilerActionKind::ResolveImports;
  }
  bool IsTypeCheckAction() const {
    return GetSelfActionKind() == CompilerActionKind::TypeCheck;
  }
  bool IsEmitASTAction() const {
    return GetSelfActionKind() == CompilerActionKind::EmitAST;
  }
  bool IsEmitIRAction() const {
    return GetSelfActionKind() == CompilerActionKind::EmitIR;
  }
  bool IsEmitBCAction() const {
    return GetSelfActionKind() == CompilerActionKind::EmitBC;
  }
  bool IsEmitObjectAction() const {
    return GetSelfActionKind() == CompilerActionKind::EmitObject;
  }
  bool IsEmitAssemblyAction() const {
    return GetSelfActionKind() == CompilerActionKind::EmitAssembly;
  }
  bool IsEmitModuleAction() const {
    return GetSelfActionKind() == CompilerActionKind::EmitModule;
  }
  bool IsMergeModulesAction() const {
    return GetSelfActionKind() == CompilerActionKind::MergeModules;
  }

public:
  static bool classof(const CompilerAction *action) { return true; }
};

class PrintHelpAction final : public CompilerAction {
public:
  PrintHelpAction(CompilerInstance &instance) : CompilerAction(instance) {}

public:
  bool ExecuteAction() override;

  CompilerActionKind GetSelfActionKind() const override {
    return CompilerActionKind::PrintHelp;
  }
};

class PrintHelpHiddenAction final : public CompilerAction {
public:
  PrintHelpHiddenAction(CompilerInstance &instance)
      : CompilerAction(instance) {}

public:
  bool ExecuteAction() override;

  CompilerActionKind GetSelfActionKind() const override {
    return CompilerActionKind::PrintHelpHidden;
  }
};

class PrintVersionAction final : public CompilerAction {

public:
  PrintVersionAction(CompilerInstance &instance) : CompilerAction(instance) {}

public:
  bool ExecuteAction() override;
  CompilerActionKind GetSelfActionKind() const override {
    return CompilerActionKind::PrintVersion;
  }
};

class PrintFeatureAction final : public CompilerAction {
public:
  PrintFeatureAction(CompilerInstance &instance) : CompilerAction(instance) {}

public:
  bool ExecuteAction() override;
  CompilerActionKind GetSelfActionKind() const override {
    return CompilerActionKind::PrintFeature;
  }
};
class ASTAction : public CompilerAction {
public:
  ASTAction(CompilerInstance &instance) : CompilerAction(instance) {}

public:
  virtual void ConsumeDecl(Decl *result) {}
  virtual void ConsumeSourceFile(SourceFile *result) {}
};

class ParseAction : public ASTAction {
public:
  ParseAction(CompilerInstance &instance) : ASTAction(instance) {}

public:
  bool ExecuteAction() override;
  CompilerActionKind GetSelfActionKind() const override {
    return CompilerActionKind::Parse;
  }
};

// class EmitParseAction final : public ASTAction {
// public:
//   EmitParseAction(CompilerInstance &instance) : ASTAction(instance) {}

// public:
//   bool ExecuteAction() override;

//   CompilerActionKind GetDepActionKind() const override {
//     return CompilerActionKind::Parse;
//   }

//   CompilerActionKind GetSelfActionKind() const override {
//     return CompilerActionKind::EmitParse;
//   }
// };

// class ResolveImportsAction final : public ASTAction {
// public:
//   ResolveImportsAction(CompilerInstance &instance) : ASTAction(instance) {}

// public:
//   bool ExecuteAction() override;

//   CompilerActionKind GetDepActionKind() const override {
//     return CompilerActionKind::Parse;
//   }

//   CompilerActionKind GetSelfActionKind() const override {
//     return CompilerActionKind::ResolveImports;
//   }
// };

class TypeCheckAction final : public ASTAction {
public:
  TypeCheckAction(CompilerInstance &instance) : ASTAction(instance) {}

public:
  bool ExecuteAction() override;

public:
  CompilerActionKind GetDepActionKind() const override {
    return CompilerActionKind::ResolveImports;
  }
  CompilerActionKind GetSelfActionKind() const override {
    return CompilerActionKind::TypeCheck;
  }
};

// class EmitASTAction final : public ASTAction {
// public:
//   EmitASTAction(CompilerInstance &instance) : ASTAction(instance) {}

// public:
//   bool ExecuteAction() override;

// public:
//   CompilerActionKind GetDepActionKind() const override {
//     return CompilerActionKind::TypeCheck;
//   }
//   CompilerActionKind GetSelfActionKind() const override {
//     return CompilerActionKind::EmitAST;
//   }
// };

class EmitCodeAction : public ASTAction {

protected:
  llvm::GlobalVariable *globalHash;

public:
  EmitCodeAction(CompilerInstance &instance) : ASTAction(instance) {}

public:
  virtual void ConsumeCodeGen(CodeGenResult *result) {}
};

class EmitIRAction final : public EmitCodeAction {

  ///\return the generated module
  CodeGenResult ExecuteAction(SourceFile &primarySourceFile,
                              llvm::StringRef moduleName,
                              const PrimaryFileSpecificPaths &sps,
                              llvm::GlobalVariable *&globalHash);

  ///\return the generated module
  CodeGenResult ExecuteAction(ModuleDecl *moduleDecl,
                              llvm::StringRef moduleName,
                              const PrimaryFileSpecificPaths &sps,
                              ArrayRef<std::string> parallelOutputFilenames,
                              llvm::GlobalVariable *&globalHash);

public:
  EmitIRAction(CompilerInstance &instance) : EmitCodeAction(instance) {}

public:
  bool ExecuteAction() override;

public:
  CompilerActionKind GetDepActionKind() const override {
    return CompilerActionKind::TypeCheck;
  }
  CompilerActionKind GetSelfActionKind() const override {
    return CompilerActionKind::EmitIR;
  }

public:
  static bool classof(const CompilerAction *action) {
    return action->IsEmitIRAction();
  }
};

class EmitObjectAction final : public EmitCodeAction {
public:
  EmitObjectAction(CompilerInstance &instance) : EmitCodeAction(instance) {}

public:
  bool ExecuteAction() override;

public:
  CompilerActionKind GetDepActionKind() const override {
    return CompilerActionKind::EmitIR;
  }
  CompilerActionKind GetSelfActionKind() const override {
    return CompilerActionKind::EmitObject;
  }
  void ConsumeCodeGen(CodeGenResult *result) override;
};

// class EmitBCAction final : public EmitCodeAction {
// public:
//   EmitBCAction(CompilerInstance &instance) : EmitCodeAction(instance) {}

// public:
//   bool ExecuteAction() override;

// public:
//   CompilerActionKind GetDepActionKind() const override {
//     return CompilerActionKind::EmitIR;
//   }
//   CompilerActionKind GetSelfActionKind() const override {
//     return CompilerActionKind::EmitBC;
//   }
//   void ConsumeEmittedCode(CodeGenResult *result) override;
// };

} // namespace stone
#endif