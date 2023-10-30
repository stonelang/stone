#ifndef STONE_COMPILE_COMPILER_H
#define STONE_COMPILE_COMPILER_H

#include "stone/Basic/Mem.h"
#include "stone/Compile/CompilerInvocation.h"
#include "stone/AST/ASTContext.h"

#include "llvm/ADT/ArrayRef.h"

namespace stone {
class CodeGenContext;
class CodeGenResult;

class CompilerInstanceStats final : public Stats {
  const CompilerInstance &compiler;

public:
  CompilerInstanceStats(const CompilerInstance &compiler)
      : Stats("CompilerInvocationstatistics:"), compiler(compiler) {}
  void Print(ColorStream &stream) override;
};

using ModuleASTFileUnion =
    llvm::PointerUnion<syn::ModuleDecl *, syn::ASTFile *>;

using ParsingCompletedCallback = llvm::function_ref<Status(syn::ASTFile &)>;

using TypeCheckingCompletedCallback =
    llvm::function_ref<Status(CompilerInstance &)>;

using CodeGenCompletedCallback =
    llvm::function_ref<Status(CompilerInstance &compiler, CodeGenContext &cgc)>;

using BackendCodeGenCompletedCallback =
    llvm::function_ref<void(CompilerInstance &)>;

using EachASTFileCallback = llvm::function_ref<void(
    syn::ASTFile &, TypeCheckerOptions &, TypeCheckerListener *)>;

// using CompileWithGenIRCallback = llvm::function_ref<void(
//     CompilerInvocation&invocation, CodeGenContext &cgc, CodeGenResult
//     &result)>;

class CompilerInstance final {
  std::unique_ptr<CompilerInstanceStats> stats;

  CompilerInvocation &invocation;

  std::unique_ptr<syn::ASTContext> sc;
  std::unique_ptr<ModuleSystem> ms;

  // /// Contains buffer IDs for input source code files.
  // std::vector<unsigned> inputSourceBufferIDs;

  // /// Identifies the set of input buffers in the SourceManager that are
  // /// considered primaries.
  // llvm::SetVector<unsigned> primaryBufferIDs;

  /// The stream for verbose output if owned, otherwise nullptr.
  // Safe<raw_ostream> OwnedVerboseOutputStream;

  // /// The stream for verbose output.
  // raw_ostream *VerboseOutputStream = &llvm::errs();

  mutable syn::ModuleDecl *mainModule = nullptr;

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
  syn::ASTContext &GetASTContext() { return *sc.get(); }
  ModuleSystem &GetModuleSystem() { return *ms.get(); }
  const ModuleSystem &GetModuleSystem() const { return *ms.get(); }

  CompilerInvocation &GetInvocation() { return invocation; }

  bool CanCompile() {
    return GetInvocation().GetCompilerOptions().GetMode().CanCompile();
  }
  bool CanCodeGen() {
    return GetInvocation().GetCompilerOptions().GetMode().CanCodeGen();
  }
  // llvm::StringRef CreateOutputFile(unsigned srcID);
  // llvm::StringRef ComputeSourceOutputFile(unsigned srcID);

public:
  /// Perform code analysis and code generation
  Status Compile();
  
  // Status CompileFrontend();
  // Status CompileBackend();

private:
  Status CompileWithParsing();
  Status CompileWithParsing(ParsingCompletedCallback notifiy);

  Status CompileWithTypeChecking();
  Status CompileWithTypeChecking(TypeCheckingCompletedCallback notifiy);

  Status CompileWithCodeGen();
  Status CompileWithGenIR(CodeGenContext &cgc,
                          CodeGenCompletedCallback notifiy);

  Status CompileWithGenNative(CodeGenContext &cgc);

public:
  void ForEachASTFile(EachASTFileCallback fn);
  void ResolveImports();

public:
  syn::ModuleDecl *CastToModuleDecl(stone::ModuleASTFileUnion msf) {
    return msf.get<syn::ModuleDecl *>();
  }
  syn::ASTFile *CastToASTFile(stone::ModuleASTFileUnion msf) {
    msf.dyn_cast<syn::ASTFile *>();
  }

  Mode &GetMode() { return invocation.GetCompilerOptions().GetMode(); }
  const Mode &GetMode() const {
    return invocation.GetCompilerOptions().GetMode();
  }

public:
  // TODO: Consider moving to the Compiler
  ModuleOutputMode GetModuleOutputMode() {
    // TODO: This must be computed in the future.
    return GetInvocation().GetModuleOptions().moduleOutputMode;
  }

  bool IsWholeModuleCodeGen() {
    return ((invocation.GetCompilerOptions()
                 .GetInputsAndOutputs()
                 .HasPrimaryInputs())
                ? false
                : true);
  }
  bool IsASTFileCodeGen() { return !GetPrimaryASTFiles().empty(); }

public:
  //== Utils ==//
  static std::unique_ptr<llvm::raw_fd_ostream>
  GetFileOutputStream(llvm::StringRef outputFilename, LangContext &ctx);

  void ComputeCodeCodeGenOutputKind();

public:
  /// Gets the set of ASTFiles which are the primary inputs for this
  /// CompilerInstance.
  llvm::ArrayRef<syn::ASTFile *> GetPrimaryASTFiles() const {
    return GetModuleSystem().GetMainModule()->GetPrimaryASTFiles();
  }

  // bool HasPrimaryASTFiles() const {
  //   return GetModuleSystem().GetMainModule()->HasPrimaryASTFiles();
  // }

  const PrimaryFileSpecificPaths &
  GetPrimaryFileSpecificPathsForWholeModuleOptimizationMode() const;

  const PrimaryFileSpecificPaths &
  GetPrimaryFileSpecificPathsForAtMostOnePrimary() const;

  const PrimaryFileSpecificPaths &
  GetPrimaryFileSpecificPathsForPrimary(StringRef fileName) const;

  const PrimaryFileSpecificPaths &
  GetPrimaryFileSpecificPathsForASTFile(const syn::ASTFile &sf) const;

public:
  void PrintHelp(const llvm::opt::OptTable &opts);
};

} // namespace stone
#endif
