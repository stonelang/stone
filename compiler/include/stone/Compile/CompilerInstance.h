#ifndef STONE_COMPILE_COMPILERINSTANCE_H
#define STONE_COMPILE_COMPILERINSTANCE_H

#include "stone/Basic/Mem.h"
#include "stone/Compile/CompilerInvocation.h"
#include "stone/Syntax/SyntaxContext.h"

#include "llvm/ADT/ArrayRef.h"

namespace stone {

class CodeGenContext;
class CompilerInstance;

class CompilerInstanceStats final : public Stats {
  const CompilerInstance &compiler;

public:
  CompilerInstanceStats(const CompilerInstance &compiler)
      : Stats("CompilerInvocationstatistics:"), compiler(compiler) {}
  void Print(ColorStream &stream) override;
};

using EachSyntaxFileToTypeCheckCallback = std::function<Status(
    syn::SyntaxFile &, TypeCheckerOptions &, TypeCheckerListener *)>;

/// A PrettyStackTraceEntry to print compiling information
class CompilerPrettyStackTrace : public llvm::PrettyStackTraceEntry {
  const CompilerInvocation &compilerInvocation;

public:
  CompilerPrettyStackTrace(CompilerInvocation &compilerInvocation)
      : compilerInvocation(compilerInvocation) {}
  void print(llvm::raw_ostream &os) const override;
};

class CompilerConfiguration {
public:
};
class CompilerInstance final : public CompilerConfiguration {

  CompilerListener *listener;
  CompilerInvocation compilerInvocation;

  std::unique_ptr<CompilerInstanceStats> compilerStats;
  std::unique_ptr<ModuleSystem> moduleSystem;
  std::unique_ptr<syn::SyntaxContext> syntaxContext;

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

public:
  CompilerInstance() = delete;
  CompilerInstance(CompilerListener *listener = nullptr);
  ~CompilerInstance();

public:
  void Initialize(const CompilerInvocation &compilerInvocation);
  void Finalize();

public:
  syn::SyntaxContext &GetSyntaxContext() { return *syntaxContext; }
  ModuleSystem &GetModuleSystem() { return *moduleSystem; }
  const ModuleSystem &GetModuleSystem() const { return *moduleSystem; }

  CompilerInvocation &GetInvocation() { return compilerInvocation; }
  CompilerListener *GetListener() { return listener; }

  bool CanCompile() {
    return GetInvocation().GetCompilerOptions().GetAction().CanCompile();
  }
  bool CanCodeGen() {
    return GetInvocation().GetCompilerOptions().GetAction().CanCodeGen();
  }

  bool IsActionPostTypeChecking() {
    switch (GetAction().GetKind()) {
    case ActionKind::EmitModule:
    case ActionKind::MergeModules:
    case ActionKind::EmitAssembly:
    case ActionKind::EmitIRAfter:
    case ActionKind::EmitIRBefore:
    case ActionKind::EmitBC:
    case ActionKind::EmitObject:
    case ActionKind::DumpTypeInfo:
      return true;
    default:
      return false;
    }
  }
  // llvm::StringRef CreateOutputFile(unsigned srcID);
  // llvm::StringRef ComputeSourceOutputFile(unsigned srcID);

public:
  /// Perform code analysis and code generation
  Status Compile();

  // Status CompileFrontend();
  // Status CompileBackend();

private:
  // Status CompileWithParsing();
  // Status CompileWithParsing(ParsingCompletedCallback notifiy);

  // Status CompileWithTypeChecking();
  // Status CompileWithTypeChecking(TypeCheckingCompletedCallback notifiy);

  // Status CompileWithCodeGen();
  // Status CompileWithGenIR(CodeGenContext &cgc,
  //                         IRCodeGenCompletedCallback notifiy);

  // Status CompileWithGenNative(CodeGenContext &cgc);

public:
  Status ForEachSyntaxFile(EachSyntaxFileCallback fn);
  Status ForEachSyntaxFileToTypeCheck(EachSyntaxFileToTypeCheckCallback notify);

  void ResolveImports();

public:
  syn::ModuleDecl *CastToModuleDecl(stone::ModuleSyntaxFileUnion msf) {
    return msf.get<syn::ModuleDecl *>();
  }
  syn::SyntaxFile *CastToSyntaxFile(stone::ModuleSyntaxFileUnion msf) {
    msf.dyn_cast<syn::SyntaxFile *>();
  }

  CompilerAction &GetAction() {
    return compilerInvocation.GetCompilerOptions().GetAction();
  }
  const CompilerAction &GetAction() const {
    return compilerInvocation.GetCompilerOptions().GetAction();
  }

public:
  // TODO: Consider moving to the Compiler
  ModuleOutputMode GetModuleOutputMode() {
    // TODO: This must be computed in the future.
    return GetInvocation().GetModuleOptions().moduleOutputMode;
  }

  bool IsWholeModuleCodeGen() {
    return ((compilerInvocation.GetCompilerOptions()
                 .GetInputsAndOutputs()
                 .HasPrimaryInputs())
                ? false
                : true);
  }
  bool IsSyntaxFileCodeGen() { return !GetPrimarySyntaxFiles().empty(); }

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

  // bool HasPrimarySyntaxFiles() const {
  //   return GetModuleSystem().GetMainModule()->HasPrimarySyntaxFiles();
  // }

  const PrimaryFileSpecificPaths &
  GetPrimaryFileSpecificPathsForWholeModuleOptimizationMode() const;

  const PrimaryFileSpecificPaths &
  GetPrimaryFileSpecificPathsForAtMostOnePrimary() const;

  const PrimaryFileSpecificPaths &
  GetPrimaryFileSpecificPathsForPrimary(StringRef fileName) const;

  const PrimaryFileSpecificPaths &
  GetPrimaryFileSpecificPathsForSyntaxFile(const syn::SyntaxFile &sf) const;

public:
  void PrintHelp(ColorStream &out, const llvm::opt::OptTable &opts);
  // void PrintVersion();
  // void PrintTimers();
  // void PrintDiagnostics();
  // void PrintStatistics();
};

} // namespace stone
#endif
