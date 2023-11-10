#ifndef STONE_COMPILE_COMPILER_H
#define STONE_COMPILE_COMPILER_H

#include "stone/Compile/CompilerCommandLine.h"

namespace stone {

class CompilerAction final : public Action {
  friend CompilerCommandLine;

public:
  CompilerCommandLine(const CompilerCommandLine &) = delete;
  void operator=(const CompilerCommandLine &) = delete;
  CompilerCommandLine(CompilerCommandLine &&) = delete;
  void operator=(CompilerCommandLine &&) = delete;

public:
  CompilerAction();
};

class CompilerCommandLine {

  CompilerAction action;

private:
  // CompilerOptions compilerOpts;

  // /// Options for generating code
  // CodeGenOptions codeGenOpts;

  // /// The options for searching libs
  // SearchPathOptions searchPathOpts;

  // /// The options for type-checking
  // TypeCheckerOptions typeCheckerOpts;

  // stone::TargetOptions targetOpts;

  // ModuleOptions moduleOpts;

  // ASTOptions astOpts;

  // /// Contains buffer IDs for input source code files.
  // std::vector<unsigned> sourceBufferIDs;

  // // The primary Sources
  // llvm::SetVector<unsigned> primarySourceIDs;

public:
  // CompilerOptions &GetCompilerOptions() { return compilerOpts; }
  // const CompilerOptions &GetCompilerOptions() const {
  //   return *compilerOpts.get();
  // }

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

protected:
  // Status ParseCompilerAction();
  // Status ParseCompilerOptions();
  // Status ParseCodeGenOptions();
  // Status ParseASTOptions();
  // Status ParseSearchPathOptions();
  // Status ParseTypeCheckerOptions();
  // Status ParseModuleOptions();
public:
  Status ParseCommandLine(llvm::ArrayRef<const char *> args);
};

class Compiler final : public CompilerCommandLine {
  bool isCompiling = false;
 
public:
  Compiler(const Compiler &) = delete;
  void operator=(const Compiler &) = delete;
  Compiler(Compiler &&) = delete;
  void operator=(Compiler &&) = delete;

public:
  Compiler();
  ~Compiler();

};

} // namespace stone
