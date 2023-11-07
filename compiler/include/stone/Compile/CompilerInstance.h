#ifndef STONE_COMPILE_COMPILER_H
#define STONE_COMPILE_COMPILER_H

#include "stone/AST/ASTContext.h"
#include "stone/Basic/Lang.h"
#include "stone/Basic/Mem.h"
#include "stone/Compile/CompilerInvocation.h"

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

using ParsingCompletedCallback = st::function<Status(ast::ASTFile &)>;

using TypeCheckingCompletedCallback = std::function<Status(CompilerInstance &)>;

using CodeGenCompletedCallback =
    std::function<Status(CompilerInstance &compiler, CodeGenContext &cgc)>;

using EachASTFileCallback = std::function<void(
    ast::ASTFile &, TypeCheckerOptions &, TypeCheckerListener *)>;

/// A PrettyStackTraceEntry to print compiling information
class CompilerPrettyStackTrace : public llvm::PrettyStackTraceEntry {
  const CompilerInvocation &invocation;

public:
  CompilerPrettyStackTrace(const CompilerInvocation &invocation)
      : invocation(invocation) {}
  void print(llvm::raw_ostream &os) const override;
};

class CompilerInstance final {
  std::unique_ptr<CompilerInstanceStats> stats;

  CompilerInvocation &invocation;

  std::unique_ptr<ast::ASTContext> sc;
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

  mutable ast::ModuleDecl *mainModule = nullptr;

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
  ast::ASTContext &GetASTContext() { return *sc.get(); }
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

private:
  Status CompileWithParsing();
  Status CompileWithParsing(ParsingCompletedCallback notifiy);
  Status CompileWithTypeChecking();
  Status CompileWithTypeChecking(TypeCheckingCompletedCallback notifiy);

private:
  Status CompileWitCodeGen();
  Status CompileWithGenIR(CodeGenContext &cgc,
                          CodeGenCompletedCallback notifiy);
  Status CompileWithGenNative(CodeGenContext &cgc);

public:
  void ForEachASTFile(EachASTFileCallback fn);
  void ResolveImports();

public:
  ast::ModuleDecl *CastToModuleDecl(stone::ModuleOrASTFile msf) {
    return msf.get<ast::ModuleDecl *>();
  }
  ast::ASTFile *CastToASTFile(stone::ModuleOrASTFile msf) {
    msf.dyn_cast<ast::ASTFile *>();
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
  GetFileOutputStream(llvm::StringRef outputFilename, Lang &ctx);

  void ComputeCodeCodeGenOutputKind();

public:
  /// Gets the set of ASTFiles which are the primary inputs for this
  /// CompilerInstance.
  llvm::ArrayRef<ast::ASTFile *> GetPrimaryASTFiles() const {
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
  GetPrimaryFileSpecificPathsForASTFile(const ast::ASTFile &sf) const;

public:
  void PrintHelp(const llvm::opt::OptTable &opts);
};

} // namespace stone
#endif
