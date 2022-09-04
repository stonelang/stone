#ifndef STONE_COMPILE_COMPILER_H
#define STONE_COMPILE_COMPILER_H

#include "stone/Compile/CompilerInvocation.h"
#include "stone/Syntax/SyntaxContext.h"

#include "llvm/ADT/ArrayRef.h"

namespace stone {

class CompilerInstance;
using ModuleSyntaxFileUnion =
    llvm::PointerUnion<syn::Module *, syn::SyntaxFile *>;

using SyntaxAnalysisCallback = llvm::function_ref<void(syn::SyntaxFile &)>;
using SemanticAnalysisCallback = llvm::function_ref<void(CompilerInstance &)>;

// using IRCodeGenCallback = llvm::function_ref<void(CompilerInstance &)>;
// using NativeCodeGenCallback = llvm::function_ref<void(CompilerInstance &)>;

using EachSyntaxFileCallback = llvm::function_ref<void(
    syn::SyntaxFile &, TypeCheckerOptions &, TypeCheckerListener *)>;

// using CompileWithGenIRCallback = llvm::function_ref<void(
//     CompilerInvocation&invocation, CodeGenContext &cgc, IRCodeGenResult
//     &result)>;

class CompilerInstanceStats final : public Stats {
  const CompilerInstance &compiler;

public:
  CompilerInstanceStats(const CompilerInstance &compiler)
      : Stats("CompilerInvocationstatistics:"), compiler(compiler) {}
  void Print(ColorfulStream &stream) override;
};

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
  void Compile();

private:
  void CompileWithSyntaxAnalysis();
  void CompileWithSyntaxAnalysis(SyntaxAnalysisCallback client);

  void CompileWithSemanticAnalysis();
  void CompileWithSemanticAnalysis(SemanticAnalysisCallback client);

  // TODO: Some things to think about
  void CompileWithIRCodeGen();
  void CompileWithNativeCodeGen();

  // void CompileWithGenIR(stone::ModuleSyntaxFileUnion msf, CodeGenContext
  // &cgc,
  //                       CompileWithGenIRCallback client);

  void ForEachSyntaxFile(EachSyntaxFileCallback client);
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
