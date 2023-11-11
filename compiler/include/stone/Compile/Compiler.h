#ifndef STONE_COMPILE_COMPILER_H
#define STONE_COMPILE_COMPILER_H

#include "stone/Basic/Basic.h"
#include "stone/Options/Action.h"
#include "stone/Options/Options.h"

#include "llvm/ADT/ArrayRef.h"

namespace stone {

class Compiling;
class CompilerCommandLine;

class CompilerAction final : public Action {

  // This allows the CompilerCommandLine to set the kind and name.
  friend CompilerCommandLine;

public:
  CompilerAction(const CompilerAction &) = delete;
  void operator=(const CompilerAction &) = delete;
  CompilerAction(CompilerAction &&) = delete;
  void operator=(CompilerAction &&) = delete;

public:
  CompilerAction();
};

class CompilerInputArgList final {
  llvm::opt::InputArgList &args;

public:
  CompilerInputArgList(llvm::opt::InputArgList args) : args(args) {}

public:
  const llvm::opt::Arg *GetActionArg();
  const unsigned GetOptionID(llvm::opt::Arg *arg);
};

class CompilerCommandLine {

  Basic basic;
  CompilerAction action;
  std::unique_ptr<llvm::opt::OptTable> compilerOptionTable;

private:
  CompilerOptions compilerOpts;

  /// Options for generating code
  CodeGenOptions codeGenOpts;

  /// The options for searching libs
  SearchPathOptions searchPathOpts;

  /// The options for type-checking
  TypeCheckerOptions typeCheckerOpts;

  stone::TargetOptions targetOpts;

  ModuleOptions moduleOpts;

  ASTOptions astOpts;

  // /// Contains buffer IDs for input source code files.
  std::vector<unsigned> sourceBufferIDs;

  // // The primary Sources
  llvm::SetVector<unsigned> primarySourceIDs;

public:
  Basic &GetBasic() { return basic; }

  CompilerOptions &GetCompilerOptions() { return compilerOpts; }
  const CompilerOptions &GetCompilerOptions() const {
    return *compilerOpts.get();
  }

protected:
  llvm::opt::OptTable &GetCompilerOptionTable() { return *compilerOptionTable; }

public:
  // CodeGenOptions &GetCodeGenOptions() { return codeGenOpts; }
  // const CodeGenOptions &GetCodeGenOptions() const { return codeGenOpts; }

  // stone::TargetOptions &GetTargetOptions() { return targetOpts; }
  // const stone::TargetOptions &GetTargetOptions() const { return targetOpts; }

  // ASTOptions &GetASTOptions() { return astOpts; }
  // const ASTOptions &GetASTOptions() const { return astOpts; }

  // TypeCheckerOptions &GetTypeCheckerOptions() { return typeCheckerOpts; }
  // const TypeCheckerOptions &GetTypeCheckerOptions() const {
  //   return typeCheckerOpts;
  // }

  // SearchPathOptions &GetSearchPathOptions() { return searchPathOpts; }
  // const SearchPathOptions &GetSearchPathOptions() const {
  //   return searchPathOpts;
  // }
  // ModuleOptions &GetModuleOptions() { return moduleOpts; }
  // const ModuleOptions &GetModuleOptions() const { return moduleOpts; }

  // TypeCheckMode GetTypeCheckMode() {
  //   return (primarySourceIDs.empty() ? TypeCheckMode::WholeModule
  //                                    : TypeCheckMode::EachFile);
  //   // TODO: Set in ParseArgs return GetTypeCheckerOptions().typeCheckMode;
  // }

  CompilerAction &GetAction() { return action; }

private:
  Status ParseCompilerAction(CompilerInputArgList &args);
  Status ParseCompilerOptions(CompilerInputArgList &args);
  Status ParseCodeGenOptions();
  Status ParseASTOptions();
  Status ParseSearchPathOptions();
  Status ParseTypeCheckerOptions();
  Status ParseModuleOptions();

public:
  bool ParseCommandLine(llvm::ArrayRef<const char *> args);
};

using CodeAnalysisCompletion = std::function<Status(ASTFile &, Compiling &)>;

class Compiling final {

  Compiler &compiler;
  std::unique_ptr<stone::ASTContext> astContext;
  std::unique_ptr<ModuleSystem> moduleSystem;
  std::unique_ptr<CodeGenContext> codeGenContext;

public:
  Compiling(Compiler &compiler);

public:
  Compiler &GetCompiler() { return compiler; }
  ASTContext &GetASTContext() { return *astContext; }
  ModuleSystem &GetModuleSystem() { return *moduleSystem; }
  CodeGenContext &GetCodeGenContext() { *codeGenContext; }

public:
  Status WithParsing();
  Status WithParsingAndImportResoltuion();
  void WithASTDumping();
  Status WithTypeChecking();

  void FinishTypeChecking();
  void WithASTPrinting();
  Status WithTypeInfoDumping();
  Status WithIRGeneration(CodeGenContext &codeGenContext);
  Status WithCodeAnalysis();

public:
  Status WithCodeGeneration(CodeGenContext &codeGenContext);
  void WitIRDumping(CodeGenContext &codeGenContext);
  void WithIRPrinting(CodeGenContext &codeGenContext);
  Status WithNativeGeneration(CodeGenContext &codeGenContext);
};

class Compiler final : public CompilerCommandLine {
  Compiling compiling;

public:
  Compiler(const Compiler &) = delete;
  void operator=(const Compiler &) = delete;
  Compiler(Compiler &&) = delete;
  void operator=(Compiler &&) = delete;

public:
  Compiler();
  ~Compiler();

private:
  Compiling &GetCompiling() { return compiling; }

public:
  bool Compile();
  void Finish();
};

// class Compiling final {
//   Compiler &compiler;
//   std::unique_ptr<stone::ASTContext> astContext;
//   std::unique_ptr<ModuleSystem> moduleSystem;

// public:
//   Compiling(Compiler &compiler) : compiler(compiler) {}

// public:
// };

} // namespace stone

#endif
