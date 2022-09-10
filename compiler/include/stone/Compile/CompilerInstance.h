#ifndef STONE_COMPILE_COMPILER_H
#define STONE_COMPILE_COMPILER_H

#include "stone/Compile/CompilerInvocation.h"
#include "stone/Syntax/SyntaxContext.h"

#include "llvm/ADT/ArrayRef.h"

namespace stone {

class CodeGenContext;
class IRCodeGenResult;

class CompilerInstanceStats final : public Stats {
  const CompilerInstance &compiler;

public:
  CompilerInstanceStats(const CompilerInstance &compiler)
      : Stats("CompilerInvocationstatistics:"), compiler(compiler) {}
  void Print(ColorfulStream &stream) override;
};

class CompileStatus final {
  unsigned isError : 1;
  unsigned IsCodeCompletion : 1;

public:
  /// Construct a successful parser status.
  CompileStatus() : isError(0), IsCodeCompletion(0) {}

  /// Construct a parser status with specified bits.
  CompileStatus(bool isError, bool isCodeCompletion = false)
      : isError(0), IsCodeCompletion(0) {
    if (isError) {
      SetIsError();
    }
    if (isCodeCompletion) {
      IsCodeCompletion = true;
    }
  }
  /// Return true if either 1) no errors were encountered while parsing this,
  /// or 2) there were errors but the the parser already recovered from them.
  bool IsSuccess() const { return !IsError(); }
  bool IsErrorOrHasCompletion() const { return isError || IsCodeCompletion; }

  /// Return true if we found a code completion token while parsing this.
  bool HasCodeCompletion() const { return IsCodeCompletion; }

  /// Return true if we encountered any errors while parsing this that the
  /// parser hasn't yet recovered from.
  bool IsError() const { return isError; }

  void SetIsError() { isError = true; }

  void SetHasCodeCompletion() { IsCodeCompletion = true; }

  void ClearIsError() { isError = false; }

  void SetHasCodeCompletionAndIsError() {
    isError = true;
    IsCodeCompletion = true;
  }
  CompileStatus &operator|=(CompileStatus RHS) {
    isError |= RHS.isError;
    IsCodeCompletion |= RHS.IsCodeCompletion;
    return *this;
  }

  friend CompileStatus operator|(CompileStatus LHS, CompileStatus RHS) {
    CompileStatus Result = LHS;
    Result |= RHS;
    return Result;
  }

public:
  static CompileStatus MakeSuccess() { return CompileStatus(); }
  static CompileStatus MakeError() {
    CompileStatus Status;
    Status.SetIsError();
    return Status;
  }
};

using ModuleSyntaxFileUnion =
    llvm::PointerUnion<syn::Module *, syn::SyntaxFile *>;

using ParsingCompletedCallback =
    llvm::function_ref<CompileStatus(syn::SyntaxFile &)>;

using TypeCheckingCompletedCallback =
    llvm::function_ref<CompileStatus(CompilerInstance &)>;

// using IRCodeGenCompletedCallback = llvm::function_ref<void(CompilerInstance
// &)>; using NativeCodeGenCompletedCallback =
// llvm::function_ref<void(CompilerInstance &)>;

using IRCodeGenCompletedCallback = llvm::function_ref<CompileStatus(
    CompilerInstance &compiler, CodeGenContext &cgc, IRCodeGenResult &result)>;

using EachSyntaxFileCallback = llvm::function_ref<void(
    syn::SyntaxFile &, TypeCheckerOptions &, TypeCheckerListener *)>;

// using CompileWithGenIRCallback = llvm::function_ref<void(
//     CompilerInvocation&invocation, CodeGenContext &cgc, IRCodeGenResult
//     &result)>;

class CompilerInstance final {

  CompilerInvocation &invocation;

  std::unique_ptr<syn::SyntaxContext> sc;
  std::unique_ptr<ModuleSystem> ms;
  std::unique_ptr<CompilerInstanceStats> stats;

  // /// Contains buffer IDs for input source code files.
  // std::vector<unsigned> inputSourceBufferIDs;

  // /// Identifies the set of input buffers in the SourceManager that are
  // /// considered primaries.
  // llvm::SetVector<unsigned> primaryBufferIDs;

public:
  CompilerInstance(const CompilerInstance &) = delete;
  void operator=(const CompilerInstance &) = delete;
  CompilerInstance(CompilerInstance &&) = delete;
  void operator=(CompilerInstance &&) = delete;

  CompilerInstance(CompilerInvocation &invocation);
  ~CompilerInstance();

public:
  void Finish();

public:
  syn::SyntaxContext &GetSyntaxContext() { return *sc.get(); }
  ModuleSystem &GetModuleSystem() { return *ms.get(); }
  CompilerInvocation &GetInvocation() { return invocation; }

  bool CanCompile() {
    return GetInvocation().GetCompilerOptions().GetMode().CanCompile();
  }

  // llvm::StringRef CreateOutputFile(unsigned srcID);
  // llvm::StringRef ComputeSourceOutputFile(unsigned srcID);

public:
  /// Perform code analysis and code generation
  CompileStatus Compile();

private:
  CompileStatus CompileWithParsing();
  CompileStatus CompileWithParsing(ParsingCompletedCallback fn);

  CompileStatus CompileWithTypeChecking();
  CompileStatus CompileWithTypeChecking(TypeCheckingCompletedCallback fn);

private:
  CompileStatus CompileWithCodeGen();
  // TODO: Some things to think about
  CompileStatus CompileWithIRCodeGen();
  CompileStatus CompileWithNativeCodeGen();

  // void CompileWithGenIR(stone::ModuleSyntaxFileUnion msf, CodeGenContext
  // &cgc,
  //                       CompileWithGenIRCallback client);

  void ForEachSyntaxFile(EachSyntaxFileCallback fn);
  void ResolveUsings();

public:
  // TODO: Consider moving to the Compiler
  ModuleOutputMode GetModuleOutputMode() {
    // TODO: This must be computed in the future.
    return GetModuleSystem().GetModuleOptions().moduleOutputMode;
  }

  void PrintHelp(const llvm::opt::OptTable &opts);

public:
  //== Utils ==//
  static std::unique_ptr<llvm::raw_fd_ostream>
  GetFileOutputStream(llvm::StringRef outputFilename, LangContext &ctx);
};

} // namespace stone
#endif
