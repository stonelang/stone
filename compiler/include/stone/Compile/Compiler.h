#ifndef STONE_COMPILE_COMPILER_H
#define STONE_COMPILE_COMPILER_H

#include "stone/Basic/FileMgr.h"
#include "stone/Compile/CompilerExecution.h"
#include "stone/Compile/CompilerInputFile.h"
#include "stone/Compile/CompilerInvocation.h"
#include "stone/Diag/DiagnosticEngine.h"
#include "stone/Stats/Stats.h"
//
// #include "llvm/Support/HashingOutputBackend.h"
// #include "llvm/Support/VirtualOutputBackend.h"

#include <deque>

namespace stone {

class ModuleDecl;
class IRGenResult;
class CompilerObservation;

class Compiler final {

  FileMgr fileMgr;
  SrcMgr srcMgr;
  DiagnosticEngine diags{srcMgr};
  StatisticEngine stats;

  std::unique_ptr<ASTContext> astContext;

  std::unique_ptr<MemoryContext> memContext;

  CompilerInvocation invocation;

  /// Contains buffer IDs for input source code files.
  std::vector<unsigned> inputSourceBufferIDList;

  /// The primary Sources
  llvm::SetVector<unsigned> primarySourceBufferIDList;

  /// Stats collections
  std::unique_ptr<CompilerStatsReporter> statsReporter;

  /// The main compiler modules
  mutable ModuleDecl *mainModule = nullptr;

  /// LLVM generated modules
  llvm::SmallVector<IRGenResult *, 8> irGenResults;

  /// Virtual OutputBackend.
  // llvm::IntrusiveRefCntPtr<llvm::vfs::OutputBackend> outputBackend = nullptr;

  CompilerObservation *observation = nullptr;

public:
  Compiler(const Compiler &) = delete;
  void operator=(const Compiler &) = delete;
  Compiler(Compiler &&) = delete;
  void operator=(Compiler &&) = delete;

public:
  Compiler();
  Status Setup();

public:
  bool HasObservation() { return observation != nullptr; }
  void SetObservation(CompilerObservation *obs) { observation = obs; }
  CompilerObservation *GetObservation() { return observation; }

  void AddDiagnosticConsumer(DiagnosticConsumer &consumer) {
    diags.AddConsumer(consumer);
  }
  void RemoveDiagnosticConsumer(DiagnosticConsumer &consumer) {
    diags.RemoveConsumer(consumer);
  }

  void AddIRGenResult(IRGenResult *result) { irGenResults.push_back(result); }
  void ForEachIRGenResult(std::function<void(IRGenResult *result)> notify) {
    for (auto result : irGenResults) {
      notify(result);
    }
  }

public:
  DiagnosticEngine &GetDiags() { return diags; }
  bool HasError() { return diags.HasError(); }
  StatisticEngine &GetStats() { return stats; }

  SrcMgr &GetSrcMgr() { return srcMgr; }
  FileMgr &GetFileMgr() { return fileMgr; }

  ASTContext &GetASTContext() { return *astContext; }
  const ASTContext &GetASTContext() const { return *astContext; }
  bool HasASTContext() const { return astContext != nullptr; }

  MemoryContext &GetMemoryContext() { return *memContext; }
  const MemoryContext &GetMemoryContext() const { return *memContext; }
  bool HasMemoryContext() const { return memContext != nullptr; }

  CompilerInvocation &GetInvocation() { return invocation; }

  Status ExecuteAction(ActionKind kind, CompilerExecution *caller = nullptr);

private:
  std::unique_ptr<CompilerExecution>
  ComputeCompilerExectution(ActionKind action);

public:
  Status SetupCompilerInputFiles();

  // TODO: You may not need this anymore
  // unsigned CreateBufferIDForCompilerInputFile(const CompilerInputFile
  // &input);

  /// Return whether there is an entry in PrimaryInputs for buffer \p BufID.
  bool IsPrimarySourceID(unsigned primarySourceID) const {
    return primarySourceBufferIDList.count(primarySourceID) != 0;
  }
  void RecordPrimarySourceID(unsigned primarySourceID);

  llvm::Optional<unsigned> CreateCodeCompletionBuffer();
  llvm::Optional<unsigned> GetRecordedBufferID(const CompilerInputFile &input,
                                               const bool shouldRecover,
                                               bool &failed);
  llvm::Optional<ModuleBuffers>
  GetInputBuffersIfPresent(const CompilerInputFile &input);

  SourceFile::ParsingOptions GetSourceFileParsingOptions(bool forPrimary) const;

  std::error_code CreateDir(std::string name) {
    return llvm::sys::fs::create_directories(name);
  }

public:
  // Module
  ModuleDecl *GetMainModule() const;
  void SetMainModule(ModuleDecl *mainModule);
  Status CreateSourceFilesForMainModule(
      ModuleDecl *mod, llvm::SmallVectorImpl<ModuleFile *> &files) const;

  SourceFile *CreateSourceFileForMainModule(ModuleDecl *mainModule,
                                            SourceFileKind fileKind,
                                            unsigned bufferID,
                                            bool isMainBuffer = false) const;

  SourceFile *ComputeMainSourceFileForModule(ModuleDecl *mod) const;

public:
  // TODO:
  //  ModuleDecl *CastToModuleDecl(stone::ModuleDeclOrSourceFile msf) {
  //    return msf.get<ModuleDecl *>();
  //  }
  //  SourceFile *CastToSourceFile(stone::ModuleDeclOrSourceFile msf) {
  //    msf.dyn_cast<SourceFile *>();
  //  }

  Status ForEachSourceFileToTypeCheck(
      std::function<Status(SourceFile &sourceFile)> notify);

  Status ForEachSourceFileInMainModule(
      std::function<Status(SourceFile &sourceFile)> notify);

  Status ForEachPrimarySourceFile(
      std::function<Status(SourceFile &sourceFile)> notify);

  bool IsCompileForWholeModule() { return primarySourceBufferIDList.empty(); }
  bool IsCompileForSourceFile() { return !GetPrimarySourceFiles().empty(); }

  void SetUpIsWholeModuleCompile() {
    if (IsCompileForWholeModule()) {
      invocation.GetCodeGenOptions().isWholeModuleCompile = true;
    } else if (IsCompileForSourceFile()) {
      invocation.GetCodeGenOptions().isWholeModuleCompile = false;
    } else {
      llvm_unreachable("Invalid IR code generation target!");
    }
  }
  bool IsWholeModuleCompile() {
    return invocation.GetCodeGenOptions().isWholeModuleCompile;
  }

  bool IsCompileForLLVMIR() {
    return GetInvocation()
        .GetCompilerOptions()
        .inputsAndOutputs.ShouldTreatAsLLVM();
  }
  /// Gets the set of SourceFiles which are the primary inputs for this
  /// CompilerInstance.
  llvm::ArrayRef<SourceFile *> GetPrimarySourceFiles() const {
    return GetMainModule()->GetPrimarySourceFiles();
  }

  ActionKind GetMainAction() { return invocation.GetMainAction().GetKind(); }

public:
  void TryFreeASTContext();
  void FreeASTContext();
  bool ShouldSetupASTContext();
  Status SetupASTContext();
  void SetupStatsReporter();

  bool TryLoadSTDLib();

  void FreeMemoryContext();

  CompilerStatsReporter &GetStatsReporter() { return *statsReporter; }

public:
  static Status IsValidModuleName(const llvm::StringRef moduleName);
};

} // namespace stone
#endif
