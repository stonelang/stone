#ifndef STONE_COMPILE_COMPILER_H
#define STONE_COMPILE_COMPILER_H

#include "stone/Basic/CodeGenOptions.h"
#include "stone/Basic/FileSystemOptions.h"
#include "stone/Basic/Mem.h"
#include "stone/Basic/ModuleOptions.h"
#include "stone/Basic/SrcLoc.h"
#include "stone/Compile/CompilerOptions.h"
#include "stone/Compile/CompilerTask.h"
#include "stone/Compile/ModuleSystem.h"
#include "stone/Gen/CodeGenContext.h"
#include "stone/Public.h"
#include "stone/Syntax/Module.h"
#include "stone/Syntax/SyntaxContext.h"
#include "stone/Syntax/SyntaxOptions.h"
#include "stone/Syntax/TypeCheckerOptions.h"

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
class CompilerConfiguration;

class CompilerStats final : public Stats {
  const Compiler &compiler;

public:
  CompilerStats(const Compiler &compiler)
      : Stats("CompilerConfigurationstatistics:"), compiler(compiler) {}
  void Print(ColorStream &stream) override;
};

using EachSyntaxFileCallback = std::function<Status(syn::SyntaxFile &)>;

using EachSyntaxFileToTypeCheckCallback = std::function<Status(
    syn::SyntaxFile &, TypeCheckerOptions &, TypeCheckerListener *)>;

/// A PrettyStackTraceEntry to print compiling information
class CompilerPrettyStackTrace : public llvm::PrettyStackTraceEntry {
  const CompilerConfiguration &config;

public:
  CompilerPrettyStackTrace(CompilerConfiguration &config) : config(config) {}
  void print(llvm::raw_ostream &os) const override;
};

class CompilerListener;

using ConfigurationFileBuffers =
    llvm::SmallVector<std::unique_ptr<llvm::MemoryBuffer>, 4>;

struct ModuleBuffers {

  std::unique_ptr<llvm::MemoryBuffer> moduleBuffer;
  std::unique_ptr<llvm::MemoryBuffer> moduleDocBuffer;
  std::unique_ptr<llvm::MemoryBuffer> moduleSourceInfoBuffer;

  // Constructor
  ModuleBuffers(
      std::unique_ptr<llvm::MemoryBuffer> moduleBuffer,
      std::unique_ptr<llvm::MemoryBuffer> moduleDocBuffer = nullptr,
      std::unique_ptr<llvm::MemoryBuffer> moduleSourceInfoBuffer = nullptr)
      : moduleBuffer(std::move(moduleBuffer)),
        moduleDocBuffer(std::move(moduleDocBuffer)),
        moduleSourceInfoBuffer(std::move(moduleSourceInfoBuffer)) {}
};

using MemoryBuffers =
    llvm::SmallVectorImpl<std::unique_ptr<llvm::MemoryBuffer>>;

class CompilerCommandLine;
class CompilerContext final {
  friend CompilerCommandLine;

private:
  CompilerOptions compilerOpts;

  /// Options for generating code
  CodeGenOptions codeGenOpts;

  /// The options for searching libs
  SearchPathOptions searchPathOpts;

  /// The options for type-checking
  TypeCheckerOptions typeCheckerOpts;

  LangOptions langOpts;

  stone::TargetOptions targetOpts;

  ModuleOptions moduleOpts;

  SyntaxOptions syntaxOpts;

  DiagnosticOptions diagOpts;

public:
  // CompilerContext() { SetTargetTriple(llvm::sys::getDefaultTargetTriple()) }

public:
  void SetTargetTriple(llvm::StringRef triple);
  void SetTargetTriple(const llvm::Triple &Triple);

public:
  LangContext &GetLangContext() { return langContext; }
  const LangContext &GetLangContext() const { return langContext; }

  ClangContext &GetClangContext() { return *clangContext; }

  CompilerOptions &GetCompilerOptions() { return compilerOpts; }
  const CompilerOptions &GetCompilerOptions() const { return compilerOpts; }

  CodeGenOptions &GetCodeGenOptions() { return codeGenOpts; }
  const CodeGenOptions &GetCodeGenOptions() const { return codeGenOpts; }

  stone::TargetOptions &GetTargetOptions() { return targetOpts; }
  const stone::TargetOptions &GetTargetOptions() const { return targetOpts; }

  SyntaxOptions &GetSyntaxOptions() { return syntaxOpts; }
  const SyntaxOptions &GetSyntaxOptions() const { return syntaxOpts; }

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

  ModuleOptions &GetModuleOptions() { return moduleOpts; }
  const ModuleOptions &GetModuleOptions() const { return moduleOpts; }

  void SetupWorkingDirectory();
};

class CompilerCommandLine {
public:
  CompilerCommandLine();

public:
  Status ParseCompilerAction(llvm::opt::InputArgList &args);
  Status ParseCompilerOptions(llvm::opt::InputArgList &args,
                              MemoryBuffers *buffers);
  Status ParseLangOptions(llvm::opt::InputArgList &args);
  Status ParseTypeCheckerOptions(llvm::opt::InputArgList &args);
  Status ParseSearchPathOptions(llvm::opt::InputArgList &args);
  Status ParseCodeGenOptions(llvm::opt::InputArgList &args);
  Status ParseTargetOptions(llvm::opt::InputArgList &args);
  llvm::StringRef ParseWorkDirectory(const llvm::opt::InputArgList &args);

public:
  static std::unique_ptr<CompilerContext>
  Parse(llvm::ArrayRef<const char *> args, const char *arg0);
};

class CompilerConfiguration final {

  CompilerOptions compilerOpts;

  /// Options for generating code
  CodeGenOptions codeGenOpts;

  /// The options for searching libs
  SearchPathOptions searchPathOpts;

  /// The options for type-checking
  TypeCheckerOptions typeCheckerOpts;

  LangOptions langOpts;

  stone::TargetOptions targetOpts;

  ModuleOptions moduleOpts;

  SyntaxOptions syntaxOpts;

  DiagnosticOptions diagOpts;

  /// Contains buffer IDs for input source code files.
  std::vector<unsigned> sourceBufferIDs;

  // The primary Sources
  llvm::SetVector<unsigned> primarySourceIDs;

  llvm::MemoryBuffer *codeCompletionBuffer = nullptr;
  /// Code completion offset in bytes from the beginning of the main
  /// source file.  Valid only if \c isCodeCompletion() == true.
  unsigned codeCompletionOffset = ~0U;

  LangContext langContext;

  std::unique_ptr<ClangContext> clangContext;

public:
  CompilerConfiguration();
  ~CompilerConfiguration();

public:
  Status ParseCommandLine(llvm::ArrayRef<const char *> args, const char *arg0);
  void SetMainExecutable(const char *arg0, void *mainAddr);
  void SetupWorkingDirectory();

public:
  Status CreateSourceBuffers();
  // TODO: You may not need this anymore
  unsigned CreateSourceBuffer(const CompilerInputFile &input);

  /// Return whether there is an entry in PrimaryInputs for buffer \p BufID.
  bool IsPrimarySourceID(unsigned primarySourceID) const {
    return primarySourceIDs.count(primarySourceID) != 0;
  }
  void RecordPrimarySourceID(unsigned primarySourceID);
  llvm::Optional<unsigned> CreateCodeCompletionBuffer();

public:
  /// Gets the set of SourceFiles which are the primary inputs for this
  /// Compiler.
  // llvm::ArrayRef<syn::SyntaxFile *> GetPrimaryFiles() const {
  //   return GetModuleSystem().GetMainModule()->GetPrimaryFiles();
  // }

  // std::unique_ptr<OutputFile> ComputeOutputFile(CompilerUnit &source);

  // TODO: update CompilerOptions
  void ComputeModuleOutputMode() { assert(false && "Not implemented"); }
  Status SetupClang(llvm::ArrayRef<const char *> args, const char *arg0);

public:
  void SetTargetTriple(llvm::StringRef triple);
  void SetTargetTriple(const llvm::Triple &Triple);

  LangContext &GetLangContext() { return langContext; }
  const LangContext &GetLangContext() const { return langContext; }

  ClangContext &GetClangContext() { return *clangContext; }

  CompilerOptions &GetCompilerOptions() { return compilerOpts; }
  const CompilerOptions &GetCompilerOptions() const { return compilerOpts; }

  CodeGenOptions &GetCodeGenOptions() { return codeGenOpts; }
  const CodeGenOptions &GetCodeGenOptions() const { return codeGenOpts; }

  stone::TargetOptions &GetTargetOptions() { return targetOpts; }
  const stone::TargetOptions &GetTargetOptions() const { return targetOpts; }

  SyntaxOptions &GetSyntaxOptions() { return syntaxOpts; }
  const SyntaxOptions &GetSyntaxOptions() const { return syntaxOpts; }

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

  ModuleOptions &GetModuleOptions() { return moduleOpts; }
  const ModuleOptions &GetModuleOptions() const { return moduleOpts; }

  TypeCheckMode GetTypeCheckMode() {
    return (primarySourceIDs.empty() ? TypeCheckMode::WholeModule
                                     : TypeCheckMode::EachFile);
    // TODO: Set in ParseArgs return GetTypeCheckerOptions().typeCheckMode;
  }

  Optional<ModuleBuffers>
  GetInputBuffersIfPresent(const CompilerInputFile &input);
  Optional<unsigned> GetRecordedBufferID(const CompilerInputFile &input,
                                         const bool shouldRecover,
                                         bool &failed);

  bool HasError() { return GetLangContext().GetDiags().HasError(); }

  std::vector<unsigned> &GetSourceBufferIDs() { return sourceBufferIDs; }
  // std::vector<unsigned> &GetPrimarySourceIDs() { return primarySourceIDs; }

  CompilerAction &GetAction() { return GetCompilerOptions().GetAction(); }

private:
  Status ParseCompilerAction(llvm::opt::InputArgList &args);
  Status ParseCompilerOptions(llvm::opt::InputArgList &args,
                              MemoryBuffers *buffers);
  Status ParseLangOptions(llvm::opt::InputArgList &args);
  Status ParseTypeCheckerOptions(llvm::opt::InputArgList &args);
  Status ParseSearchPathOptions(llvm::opt::InputArgList &args);
  Status ParseCodeGenOptions(llvm::opt::InputArgList &args);
  Status ParseTargetOptions(llvm::opt::InputArgList &args);
  llvm::StringRef ParseWorkDirectory(const llvm::opt::InputArgList &args);

public:
  void PrintHelp();
};

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

class Compiler final /*: public CompilerContext*/ {
  friend CompilerTask;
  friend CompilerCommandLineParser;

  CompilerListener *listener;
  std::unique_ptr<CompilerStats> stats;
  std::unique_ptr<ModuleSystem> moduleSystem;
  std::unique_ptr<syn::SyntaxContext> syntaxContext;

  /// Runs tasks
  CompilerQueue queue;

  /// Configures the compiler
  CompilerConfiguration config;

  // llvm::sys::TimePoint<> startTime;
  // llvm::sys::TimePoint<> endTime = llvm::sys::TimePoint<>::min();

  // /// Contains buffer IDs for input source code files.
  // std::vector<unsigned> inputSourceBufferIDs;

  // /// Identifies the set of input buffers in the SourceManager that are
  // /// considered primaries.
  // llvm::SetVector<unsigned> primaryBufferIDs;

  /// The stream for verbose output if owned, otherwise nullptr.
  // Safe<raw_ostream> OwnedVerboseOutputStream;

  // /// The stream for verbose output.
  // raw_ostream *VerboseOutputStream = &llvm::errs();

  mutable llvm::BumpPtrAllocator bumpAlloc;

public:
  Compiler();
  ~Compiler();

public:
  void Setup();
  void Finish();

  void BuildTasks();
  void RunTasks();

private:
  void AddTask(ActionKind kind);

public:
  // TODO: May want to pass by pointer
  void SetupDiagnostics(DiagnosticConsumer &listener);
  void SetListener(CompilerListener *listener);
  CompilerListener *GetListener() { return listener; }

  void SetupSyntaxContext();
  void SetupOutputBackend();

public:
  CompilerQueue &GetQueue() { return queue; }
  CompilerConfiguration &GetConfig() { return config; }
  const CompilerConfiguration &GetConfig() const { return config; }
  llvm::BumpPtrAllocator &GetAllocator() const { return bumpAlloc; }

  DiagnosticEngine &GetDiags() {
    return GetConfig().GetLangContext().GetDiags();
  }
  StatisticEngine &GetStats() {
    return GetConfig().GetLangContext().GetStats();
  }
  bool HasError() { return GetConfig().GetLangContext().GetDiags().HasError(); }

public:
  syn::SyntaxContext &GetSyntaxContext() { return *syntaxContext; }
  ModuleSystem &GetModuleSystem() { return *moduleSystem; }
  const ModuleSystem &GetModuleSystem() const { return *moduleSystem; }

  bool CanCompile() const {
    return GetConfig().GetCompilerOptions().GetAction().CanCompile();
  }
  bool CanCodeGen() const {
    return GetConfig().GetCompilerOptions().GetAction().CanCodeGen();
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
  void ResolveImports();

public:
  Status ForEachSyntaxFile(EachSyntaxFileCallback fn);
  Status ForEachSyntaxFileToTypeCheck(EachSyntaxFileToTypeCheckCallback notify);

public:
  syn::ModuleDecl *CastToModuleDecl(stone::ModuleSyntaxFileUnion msf) {
    return msf.get<syn::ModuleDecl *>();
  }
  syn::SyntaxFile *CastToSyntaxFile(stone::ModuleSyntaxFileUnion msf) {
    msf.dyn_cast<syn::SyntaxFile *>();
  }

  CompilerAction &GetAction() {
    return GetConfig().GetCompilerOptions().GetAction();
  }
  const CompilerAction &GetAction() const {
    return GetConfig().GetCompilerOptions().GetAction();
  }

public:
  // TODO: Consider moving to the Compiler
  ModuleOutputMode GetModuleOutputMode() {
    // TODO: This must be computed in the future.
    return GetConfig().GetModuleOptions().moduleOutputMode;
  }

  bool IsWholeModuleCodeGen() {
    return ((GetConfig()
                 .GetCompilerOptions()
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
  /// Compiler.
  llvm::ArrayRef<syn::SyntaxFile *> GetPrimarySyntaxFiles() const {
    return GetModuleSystem().GetMainModule()->GetPrimarySyntaxFiles();
  }

  // bool HasPrimarySyntaxFiles() const {
  //   return GetModuleSystem().GetMainModule()->HasPrimarySyntaxFiles();
  // }

public:
  const PrimaryFileSpecificPaths &
  GetPrimaryFileSpecificPathsForWholeModuleOptimizationMode() const;

  const PrimaryFileSpecificPaths &
  GetPrimaryFileSpecificPathsForAtMostOnePrimary() const;

  const PrimaryFileSpecificPaths &
  GetPrimaryFileSpecificPathsForPrimary(StringRef fileName) const;

  const PrimaryFileSpecificPaths &
  GetPrimaryFileSpecificPathsForSyntaxFile(const syn::SyntaxFile &sf) const;

public:
  void PrintTimers();
  void PrintDiagnostics();
  void PrintStatistics();

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
