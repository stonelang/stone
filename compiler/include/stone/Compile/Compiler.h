#ifndef STONE_COMPILE_COMPILER_H
#define STONE_COMPILE_COMPILER_H

#include "stone/Basic/Mem.h"
#include "stone/Basic/SrcLoc.h"
#include "stone/Compile/CompilerInvocation.h"
#include "stone/Compile/CompilerTask.h"
#include "stone/Compile/ModuleSystem.h"
#include "stone/Gen/CodeGenContext.h"
#include "stone/Public.h"
#include "stone/Syntax/ASTContext.h"
#include "stone/Syntax/Module.h"

#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/CompilerInvocation.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/SetVector.h"
#include "llvm/Option/ArgList.h"

#include <deque>
#include <queue>

namespace llvm {
class raw_pwrite_stream;
class GlobalVariable;
class MemoryBuffer;
class Module;
class TargetOptions;
class TargetMachine;
} // namespace llvm

namespace stone {

class Compiler;
class CompilerTask;
class CodeGenContext;
class CompilerListener;

// using EachASTFileCallback = std::function<Status(syn::ASTFile &)>;

// using EachASTFileToTypeCheckCallback = std::function<Status(
//     syn::ASTFile &, TypeCheckerOptions &, TypeCheckerListener *)>;

/// The compiler stats
class CompilerStats final : public Stats {
  const Compiler &compiler;

public:
  CompilerStats(const Compiler &compiler)
      : Stats("Compiler statistics:"), compiler(compiler) {}
  void Print(ColorStream &stream) override;
};

/// A PrettyStackTraceEntry to print compiling information
class CompilerPrettyStackTrace : public llvm::PrettyStackTraceEntry {
  const Compiler &compiler;

public:
  CompilerPrettyStackTrace(Compiler &compiler) : compiler(compiler) {}
  void print(llvm::raw_ostream &os) const override;
};

// class CompilerConfiguration final {

//   CompilerOptions compilerOpts;

//   /// Options for generating code
//   CodeGenOptions codeGenOpts;

//   /// The options for searching libs
//   SearchPathOptions searchPathOpts;

//   /// The options for type-checking
//   TypeCheckerOptions typeCheckerOpts;

//   LangOptions langOpts;

//   stone::TargetOptions targetOpts;

//   ModuleOptions moduleOpts;

//   ASTOptions syntaxOpts;

//   DiagnosticOptions diagOpts;

//   /// Contains buffer IDs for input source code files.
//   std::vector<unsigned> sourceBufferIDs;

//   // The primary Sources
//   llvm::SetVector<unsigned> primarySourceBufferIDs;

//   llvm::MemoryBuffer *codeCompletionBuffer = nullptr;
//   /// Code completion offset in bytes from the beginning of the main
//   /// source file.  Valid only if \c isCodeCompletion() == true.
//   unsigned codeCompletionOffset = ~0U;

//   LangContext langContext;

//   std::unique_ptr<ClangContext> clangContext;

// public:
//   CompilerConfiguration();
//   ~CompilerConfiguration();

// public:
//   Status ParseCommandLine(llvm::ArrayRef<const char *> args, const char
//   *arg0); void SetMainExecutable(const char *arg0, void *mainAddr); void
//   SetupWorkingDirectory();

// public:
//   Status CreateSourceBuffers();
//   // TODO: You may not need this anymore
//   unsigned CreateSourceBuffer(const CompilerInputFile &input);

//   /// Return whether there is an entry in PrimaryInputs for buffer \p BufID.
//   bool IsPrimarySourceID(unsigned primarySourceID) const {
//     return primarySourceBufferIDs.count(primarySourceID) != 0;
//   }
//   void RecordPrimarySourceID(unsigned primarySourceID);
//   llvm::Optional<unsigned> CreateCodeCompletionBuffer();

// public:
//   /// Gets the set of SourceFiles which are the primary inputs for this
//   /// Compiler.
//   // llvm::ArrayRef<syn::ASTFile *> GetPrimaryFiles() const {
//   //   return GetModuleSystem().GetMainModule()->GetPrimaryFiles();
//   // }

//   // std::unique_ptr<OutputFile> ComputeOutputFile(CompilerUnit &source);

//   // TODO: update CompilerOptions
//   void ComputeModuleOutputMode() { assert(false && "Not implemented"); }
//   Status SetupClang(llvm::ArrayRef<const char *> args, const char *arg0);

// public:
//   void SetTargetTriple(llvm::StringRef triple);
//   void SetTargetTriple(const llvm::Triple &Triple);

//   LangContext &GetLangContext() { return langContext; }
//   const LangContext &GetLangContext() const { return langContext; }

//   ClangContext &GetClangContext() { return *clangContext; }

//   CompilerOptions &GetCompilerOptions() { return compilerOpts; }
//   const CompilerOptions &GetCompilerOptions() const { return compilerOpts; }

//   CodeGenOptions &GetCodeGenOptions() { return codeGenOpts; }
//   const CodeGenOptions &GetCodeGenOptions() const { return codeGenOpts; }

//   stone::TargetOptions &GetTargetOptions() { return targetOpts; }
//   const stone::TargetOptions &GetTargetOptions() const { return targetOpts; }

//   ASTOptions &GetASTOptions() { return syntaxOpts; }
//   const ASTOptions &GetASTOptions() const { return syntaxOpts; }

//   TypeCheckerOptions &GetTypeCheckerOptions() { return typeCheckerOpts; }
//   const TypeCheckerOptions &GetTypeCheckerOptions() const {
//     return typeCheckerOpts;
//   }

//   SearchPathOptions &GetSearchPathOptions() { return searchPathOpts; }
//   const SearchPathOptions &GetSearchPathOptions() const {
//     return searchPathOpts;
//   }

//   DiagnosticOptions &GetDiagnosticOptions() { return diagOpts; }
//   const DiagnosticOptions &GetDiagnosticOptions() const { return diagOpts; }

//   ModuleOptions &GetModuleOptions() { return moduleOpts; }
//   const ModuleOptions &GetModuleOptions() const { return moduleOpts; }

//   TypeCheckMode GetTypeCheckMode() {
//     return (primarySourceBufferIDs.empty() ? TypeCheckMode::WholeModule
//                                      : TypeCheckMode::EachFile);
//     // TODO: Set in ParseArgs return GetTypeCheckerOptions().typeCheckMode;
//   }

//   Optional<ModuleBuffers>
//   GetInputBuffersIfPresent(const CompilerInputFile &input);
//   Optional<unsigned> GetRecordedBufferID(const CompilerInputFile &input,
//                                          const bool shouldRecover,
//                                          bool &failed);

//   bool HasError() { return GetLangContext().GetDiags().HasError(); }

//   std::vector<unsigned> &GetSourceBufferIDs() { return sourceBufferIDs; }
//   // std::vector<unsigned> &GetPrimarySourceIDs() { return
//   primarySourceBufferIDs;
//   }

//   CompilerAction &GetAction() { return GetCompilerOptions().GetAction(); }

// private:
//   Status ParseCompilerAction(llvm::opt::InputArgList &args);
//   Status ParseCompilerOptions(llvm::opt::InputArgList &args,
//                               MemoryBuffers *buffers);
//   Status ParseLangOptions(llvm::opt::InputArgList &args);
//   Status ParseTypeCheckerOptions(llvm::opt::InputArgList &args);
//   Status ParseSearchPathOptions(llvm::opt::InputArgList &args);
//   Status ParseCodeGenOptions(llvm::opt::InputArgList &args);
//   Status ParseTargetOptions(llvm::opt::InputArgList &args);
//   llvm::StringRef ParseWorkDirectory(const llvm::opt::InputArgList &args);

// public:
//   void PrintHelp();
// };

class CompilerQueue final {

  Compiler &compiler;
  // Simple queue for now.
  std::deque<CompilerTask *> runQueue;

public:
  CompilerQueue(Compiler &compiler);

public:
  void AddTask(CompilerTask *task);
  void RemoveTask();
  void RunTasks();

public:
  Compiler &GetCompiler() { return compiler; }
};

class Compiler final {
  friend CompilerTask;

  CompilerListener *listener;

  std::unique_ptr<CompilerStats> compilerStats;
  std::unique_ptr<ModuleSystem> moduleSystem;
  std::unique_ptr<syn::ASTContext> syntaxContext;
  std::unique_ptr<ClangContext> clangContext;
  std::unique_ptr<CompilerQueue> compilerQueue;
  std::unique_ptr<CodeGenContext> codeGenContext;
  std::unique_ptr<StatisticEngine> stats;

  const CompilerInvocation &invocation;

  mutable llvm::BumpPtrAllocator allocator;

  llvm::sys::TimePoint<> startTime;

  llvm::sys::TimePoint<> endTime = llvm::sys::TimePoint<>::min();

  /// Contains buffer IDs for input source code files.
  std::vector<unsigned> inputSourceBufferIDs;

  /// Identifies the set of input buffers in the SourceManager that are
  /// considered primaries.
  llvm::SetVector<unsigned> primarySourceBufferIDs;

  llvm::MemoryBuffer *codeCompletionBuffer = nullptr;
  /// Code completion offset in bytes from the beginning of the main
  /// source file.  Valid only if \c isCodeCompletion() == true.
  unsigned codeCompletionOffset = ~0U;

  /// The stream for verbose output if owned, otherwise nullptr.
  std::unique_ptr<raw_ostream> ownedVerboseOutputStream;

  /// The stream for verbose output.
  raw_ostream *verboseOutputStream = &llvm::errs();

public:
  Compiler() = delete;
  Compiler(const CompilerInvocation &invocation);
  ~Compiler();
  Status Initialize();
  void Finalize();

public:
  Compiler(const Compiler &) = delete;
  void operator=(const Compiler &) = delete;
  Compiler(Compiler &&) = delete;
  void operator=(Compiler &&) = delete;

public:
  void BuildTasks();
  void QueueTask(ActionKind source);
  void RunTasks();

private:
  void QueueTask(CompilerTask *task) { GetQueue().AddTask(task); }

public:
  void SetupASTContext();
  void SetupOutputBackend();
  void SetupCodeGenContext();
  void SetupModules();
  void AddDiagnosticConsumer(DiagnosticConsumer &consumer);

public:
  CompilerQueue &GetQueue() { return *compilerQueue; }
  llvm::BumpPtrAllocator &GetAllocator() const { return allocator; }
  CodeGenContext &GetCodeGenContext() { return *codeGenContext; }
  syn::ASTContext &GetASTContext() { return *syntaxContext; }
  ModuleSystem &GetModuleSystem() { return *moduleSystem; }
  ClangContext &GetClangContext() { return *clangContext; }

  const ModuleSystem &GetModuleSystem() const { return *moduleSystem; }

  StatisticEngine &GetStats() { return stats; }

  CompilerAction &GetAction() { return GetCompilerOptions().GetAction(); }
  const CompilerAction &GetAction() const {
    return GetCompilerOptions().GetAction();
  }

  void SetListener(CompilerListener *inputListener) {
    listener = inputListener;
  }
  CompilerListener *GetListener() { return listener; }
  CompilerTask *GetTask(CompilerTaskKind kind);

  bool HasError() { return invocation.GetDiags().HasError(); }

public:
  // Source buffers
  Status SetupSourceBuffers();
  unsigned CreateSourceBuffer(const CompilerInputFile &input);

  /// Return whether there is an entry in PrimaryInputs for buffer \p BufID.
  bool IsPrimarySourceID(unsigned primarySourceID) const {
    return primarySourceBufferIDs.count(primarySourceID) != 0;
  }
  void RecordPrimarySourceID(unsigned primarySourceID);
  llvm::Optional<unsigned> CreateCodeCompletionBuffer();

  /// Gets the set of SourceFiles which are the primary inputs for this
  /// Compiler.
  // llvm::ArrayRef<syn::ASTFile *> GetPrimaryFiles() const {
  //   return GetModuleSystem().GetMainModule()->GetPrimaryFiles();
  // }

  std::vector<unsigned> &GetInputSourceBufferIDs() {
    return inputSourceBufferIDs;
  }
  llvm::SetVector<unsigned> &GetPrimaryBufferSourceIDs() {
    return primarySourceBufferIDs;
  }

  Optional<ModuleBuffers>
  GetInputBuffersIfPresent(const CompilerInputFile &input);

  Optional<unsigned> GetRecordedBufferID(const CompilerInputFile &input,
                                         const bool shouldRecover,
                                         bool &failed);

  raw_ostream *GetVerboseOutputStream() { return verboseOutputStream; }

public:
  //   StatisticEngine &GetStats() {
  //     return GetConfig().GetLangContext().GetStats();
  //   }
  //   GetConfig().GetLangContext().GetDiags().HasError(); }

public:
  //   bool CanCompile() const {
  //     return GetConfig().GetCompilerOptions().GetAction().CanCompile();
  //   }
  //   bool CanCodeGen() const {
  //     return GetConfig().GetCompilerOptions().GetAction().CanCodeGen();
  //   }

  //   bool IsActionPostTypeChecking() {
  //     switch (GetAction().GetKind()) {
  //     case ActionKind::EmitModule:
  //     case ActionKind::MergeModules:
  //     case ActionKind::EmitAssembly:
  //     case ActionKind::EmitIRAfter:
  //     case ActionKind::EmitIRBefore:
  //     case ActionKind::EmitBC:
  //     case ActionKind::EmitObject:
  //     case ActionKind::DumpTypeInfo:
  //       return true;
  //     default:
  //       return false;
  //     }
  //   }

  //   // llvm::StringRef CreateOutputFile(unsigned srcID);
  //   // llvm::StringRef ComputeSourceOutputFile(unsigned srcID);

  // public:
  //   /// Perform code analysis and code generation
  //   Status Compile();
  //   void ResolveImports();

  // public:
  //   Status ForEachASTFile(EachASTFileCallback fn);
  //   Status ForEachASTFileToTypeCheck(EachASTFileToTypeCheckCallback
  //   notify);

  // public:
  //   syn::ModuleDecl *CastToModuleDecl(stone::ModuleOrASTFile msf) {
  //     return msf.get<syn::ModuleDecl *>();
  //   }
  //   syn::ASTFile *CastToASTFile(stone::ModuleOrASTFile msf) {
  //     msf.dyn_cast<syn::ASTFile *>();
  //   }

  // public:
  //   // TODO: Consider moving to the Compiler
  //   ModuleOutputMode GetModuleOutputMode() {
  //     // TODO: This must be computed in the future.
  //     return GetConfig().GetModuleOptions().moduleOutputMode;
  //   }

  //   bool IsWholeModuleCodeGen() {
  //     return ((GetConfig()
  //                  .GetCompilerOptions()
  //                  .GetInputsAndOutputs()
  //                  .HasPrimaryInputs())
  //                 ? false
  //                 : true);
  //   }
  //   bool IsASTFileCodeGen() { return !GetPrimaryASTFiles().empty(); }

  // public:
  //   //== Utils ==//
  //   static std::unique_ptr<llvm::raw_fd_ostream>
  //   GetFileOutputStream(llvm::StringRef outputFilename, LangContext &ctx);

  //   void ComputeCodeCodeGenOutputKind();

  // public:
  //   /// Gets the set of ASTFiles which are the primary inputs for this
  //   /// Compiler.
  //   llvm::ArrayRef<syn::ASTFile *> GetPrimaryASTFiles() const {
  //     return GetModuleSystem().GetMainModule()->GetPrimaryASTFiles();
  //   }

  //   // bool HasPrimaryASTFiles() const {
  //   //   return GetModuleSystem().GetMainModule()->HasPrimaryASTFiles();
  //   // }

public:
  // Modules
  void SetupModules();

public:
  // Specific file paths
  const PrimaryFileSpecificPaths &
  GetPrimaryFileSpecificPathsForWholeModuleOptimizationMode() const;

  const PrimaryFileSpecificPaths &
  GetPrimaryFileSpecificPathsForAtMostOnePrimary() const;

  const PrimaryFileSpecificPaths &
  GetPrimaryFileSpecificPathsForPrimary(StringRef fileName) const;

  const PrimaryFileSpecificPaths &
  GetPrimaryFileSpecificPathsForASTFile(const syn::ASTFile &sf) const;

  // public:
  void PrintTimers();
  void PrintDiagnostics();
  void PrintStatistics();

public:
  void FreeASTContext();

public:
  /// Return the total amount of physical memory allocated for representing
  /// AST nodes and type information.
  size_t GetTotalMemUsed() const;
  void *Allocate(size_t size, unsigned align = 8) const {
    return GetAllocator().Allocate(size, align);
  }
  template <typename T> T *Allocate(size_t num = 1) const {
    return static_cast<T *>(Allocate(num * sizeof(T), alignof(T)));
  }
  void Deallocate(void *Ptr) const {}
};

} // namespace stone
#endif
