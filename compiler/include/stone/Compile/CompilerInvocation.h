#ifndef STONE_COMPILE_COMPILERINVOCATION_H
#define STONE_COMPILE_COMPILERINVOCATION_H

#include "stone/Basic/Status.h"
#include "stone/Compile/CompilerOptions.h"
#include "stone/Basic/CodeGenOptions.h"
#include "stone/Basic/LangOptions.h"
#include "stone/Diag/DiagnosticOptions.h"
#include "stone/Syntax/ASTOptions.h"
#include "stone/Syntax/TypeCheckerOptions.h"

#include "stone/Syntax/SearchPath.h"

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
  Status ParseCommandLine(llvm::ArrayRef<const char *> args);

public:
  CompilerOptions &GetCompilerOptions() { return compilerOpts; }
  const CompilerOptions &GetCompilerOptions() const { return compilerOpts; }
};

} // namespace stone
#endif
