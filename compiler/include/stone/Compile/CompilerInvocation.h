#ifndef STONE_COMPILE_COMPILERINVOCATION_H
#define STONE_COMPILE_COMPILERINVOCATION_H

#include "stone/Basic/CodeGenOptions.h"
#include "stone/Basic/LangOptions.h"
#include "stone/Basic/Status.h"
#include "stone/Compile/CompilerOptions.h"
#include "stone/Diag/DiagnosticOptions.h"
#include "stone/Syntax/ASTOptions.h"
#include "stone/Syntax/SearchPath.h"
#include "stone/Syntax/TypeCheckerOptions.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/SetVector.h"
#include "llvm/Option/ArgList.h"

namespace stone {

class Compiler;
class CompilerInvocation final {
  Compiler &compiler;

  CompilerOptions compilerOpts;

  /// Options for generating code
  CodeGenOptions codeGenOpts;

  /// The options for searching libs
  SearchPathOptions searchPathOpts;

  /// The options for type-checking
  TypeCheckerOptions typeCheckerOpts;

  LangOptions langOpts;

  ASTOptions astOpts;

  DiagnosticOptions diagOpts;

public:
  CompilerInvocation(Compiler &compiler) : compiler(compiler) {}

public:
  CompilerInvocation();

public:
  CompilerOptions &GetCompilerOptions() { return compilerOpts; }
  const CompilerOptions &GetCompilerOptions() const { return compilerOpts; }

  CodeGenOptions &GetCodeGenOptions() { return codeGenOpts; }
  const CodeGenOptions &GetCodeGenOptions() const { return codeGenOpts; }

  LangOptions &GetLangOptions() { return langOpts; }
  const LangOptions &GetLangOptions() const { return langOpts; }

  ASTOptions &GetASTOptions() { return astOpts; }
  const ASTOptions &GetASTOptions() const { return astOpts; }

  TypeCheckerOptions &GetTypeCheckerOptions() { return typeCheckerOpts; }
  const TypeCheckerOptions &GetTypeCheckerOptions() const {
    return typeCheckerOpts;
  }
  SearchPathOptions &GetSearchPathOptions() { return searchPathOpts; }
  const SearchPathOptions &GetSearchPathOptions() const {
    return searchPathOpts;
  }

  DiagnosticOptions &GetDiagnosticOptions() { return diagOpts; }
  const DiagnosticOptions &GetDiagnosticOptions() const { return diagOpts; }

  bool HasAction() { return !compilerOpts.mainAction.IsAlien(); }

public:
  Status ParseCommandLine(llvm::ArrayRef<const char *> args);
};

} // namespace stone
#endif
