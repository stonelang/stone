#ifndef STONE_COMPILE_COMPILER_H
#define STONE_COMPILE_COMPILER_H

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
class Compiler;
class ModuleDecl;
class CompilerExecution;
class GenerateIRExecution;

class Compiler final {
  friend CompilerExecution;
  friend GenerateIRExecution;

  FileMgr fileMgr;
  SrcMgr srcMgr;
  DiagnosticEngine diags{srcMgr};
  StatisticEngine stats;

  std::unique_ptr<ASTContext> astContext;
  mutable ModuleDecl *mainModule = nullptr;
  CompilerInvocation invocation;

  /// Contains buffer IDs for input source code files.
  std::vector<unsigned> inputSourceBufferIDList;
  // The primary Sources
  llvm::SetVector<unsigned> primarySourceBufferIDList;

  std::unique_ptr<SystemStatisticEngine> statistics;

  IRCodeGenResult irCodeGenResult;

  /// Virtual OutputBackend.
  // llvm::IntrusiveRefCntPtr<llvm::vfs::OutputBackend> OutputBackend = nullptr;

public:
  Compiler(const Compiler &) = delete;
  void operator=(const Compiler &) = delete;
  Compiler(Compiler &&) = delete;
  void operator=(Compiler &&) = delete;
  Compiler();
  Status Setup();

public:
  void AddDiagnosticConsumer(DiagnosticConsumer &consumer) {
    diags.AddConsumer(consumer);
  }
  void RemoveDiagnosticConsumer(DiagnosticConsumer &consumer) {
    diags.RemoveConsumer(consumer);
  }

private:
  void SetIRCodeGenResult(IRCodeGenResult codeGenResult) {
    irCodeGenResult = codeGenResult
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

  IRCodeGenResult &GetIRCodeGenResult() { return irCodeGenResult; }
  CompilerInvocation &GetInvocation() { return invocation; }
  
  std::unique_ptr<CompilerExecution> GetExecutionForAction(ActionKind kind);

  Status ExecuteAction(ActionKind kind);

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
  ModuleDecl *CastToModuleDecl(ModuleOrSourceFile msf) {
    return msf.get<ModuleDecl *>();
  }
  SourceFile *CastToSourceFile(ModuleOrSourceFile msf) {
    msf.dyn_cast<SourceFile *>();
  }

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

public:
  void TryFreeASTContext();
  void FreeASTContext();
  bool ShouldSetupASTContext();
  Status SetupASTContext();

  SystemStatisticEngine &GetStatistics() { return *statistics; }

public:
  static Status IsValidModuleName(const llvm::StringRef moduleName);
};

} // namespace stone
#endif
