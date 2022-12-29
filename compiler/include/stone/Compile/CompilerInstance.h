#ifndef STONE_COMPILE_COMPILER_H
#define STONE_COMPILE_COMPILER_H

#include "stone/Basic/Mem.h"
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

using ModuleSyntaxFileUnion =
    llvm::PointerUnion<syn::ModuleDecl *, syn::SyntaxFile *>;

using ParsingCompletedCallback = llvm::function_ref<Status(syn::SyntaxFile &)>;

using TypeCheckingCompletedCallback =
    llvm::function_ref<Status(CompilerInstance &)>;

using IRCodeGenCompletedCallback =
    llvm::function_ref<Status(CompilerInstance &compiler, CodeGenContext &cgc)>;

using BackendCodeGenCompletedCallback =
    llvm::function_ref<void(CompilerInstance &)>;

using EachSyntaxFileCallback = llvm::function_ref<void(
    syn::SyntaxFile &, TypeCheckerOptions &, TypeCheckerListener *)>;

// using CompileWithGenIRCallback = llvm::function_ref<void(
//     CompilerInvocation&invocation, CodeGenContext &cgc, IRCodeGenResult
//     &result)>;

class CompilerInstance final {
  Safe<CompilerInstanceStats> stats;

  CompilerInvocation &invocation;

  Safe<syn::SyntaxContext> sc;
  Safe<ModuleSystem> ms;

  // /// Contains buffer IDs for input source code files.
  // std::vector<unsigned> inputSourceBufferIDs;

  // /// Identifies the set of input buffers in the SourceManager that are
  // /// considered primaries.
  // llvm::SetVector<unsigned> primaryBufferIDs;

  /// The stream for verbose output if owned, otherwise nullptr.
  // Safe<raw_ostream> OwnedVerboseOutputStream;

  // /// The stream for verbose output.
  // raw_ostream *VerboseOutputStream = &llvm::errs();

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
  const ModuleSystem &GetModuleSystem() const { return *ms.get(); }
  CompilerInvocation &GetInvocation() { return invocation; }

  bool CanCompile() {
    return GetInvocation().GetCompilerOptions().GetMode().CanCompile();
  }
  // llvm::StringRef CreateOutputFile(unsigned srcID);
  // llvm::StringRef ComputeSourceOutputFile(unsigned srcID);

public:
  /// Perform code analysis and code generation
  Status Compile();

private:
  Status CompileWithParsing();
  Status CompileWithParsing(ParsingCompletedCallback notifiy);

  Status CompileWithTypeChecking();
  Status CompileWithTypeChecking(TypeCheckingCompletedCallback notifiy);

  Status CompileWithCodeGen();
  Status CompileWithGenIR(CodeGenContext &cgc,
                          IRCodeGenCompletedCallback notifiy);

  Status CompileWithGenNative(CodeGenContext &cgc);

public:
  void ForEachSyntaxFile(EachSyntaxFileCallback fn);
  void ResolveImports();

public:
  syn::ModuleDecl *CastToModuleDecl(stone::ModuleSyntaxFileUnion msf) {
    return msf.get<syn::ModuleDecl *>();
  }
  syn::SyntaxFile *CastToSyntaxFile(stone::ModuleSyntaxFileUnion msf) {
    msf.dyn_cast<syn::SyntaxFile *>();
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

public:
  //== Utils ==//
  static std::unique_ptr<llvm::raw_fd_ostream>
  GetFileOutputStream(llvm::StringRef outputFilename, LangContext &ctx);

  void ComputeCodeCodeGenOutputKind();

public:
  /// Gets the set of SyntaxFiles which are the primary inputs for this
  /// CompilerInstance.
  llvm::ArrayRef<syn::SyntaxFile *> GetPrimarySyntaxFiles() const {
    return GetModuleSystem().GetMainModule()->GetPrimarySyntaxFiles();
  }

  const PrimaryFileSpecificPaths &
  GetPrimaryFileSpecificPathsForWholeModuleOptimizationMode() const;

  const PrimaryFileSpecificPaths &
  GetPrimaryFileSpecificPathsForAtMostOnePrimary() const;

  const PrimaryFileSpecificPaths &
  GetPrimaryFileSpecificPathsForPrimary(StringRef fileName) const;

  const PrimaryFileSpecificPaths &
  GetPrimaryFileSpecificPathsForSyntaxFile(const syn::SyntaxFile &sf) const;

public:
  void PrintHelp(const llvm::opt::OptTable &opts);
};

} // namespace stone
#endif
