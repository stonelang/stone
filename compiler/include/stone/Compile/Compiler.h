#ifndef STONE_COMPILE_COMPILER_H
#define STONE_COMPILE_COMPILER_H

#include "stone/Compile/CompilerExecution.h"
#include "stone/Compile/CompilerInputFile.h"
#include "stone/Compile/CompilerInvocation.h"
#include "stone/Diag/DiagnosticEngine.h"
#include "stone/Public.h"
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

  FileMgr fileMgr;
  SrcMgr srcMgr;
  DiagnosticEngine diags{srcMgr};
  StatisticEngine stats;

  std::unique_ptr<ASTContext> astContext;
  std::unique_ptr<MemoryContext> memContext;

  CompilerInvocation invocation;

  /// Contains buffer IDs for input source code files.
  std::vector<unsigned> inputSourceBufferIDList;
  // The primary Sources
  llvm::SetVector<unsigned> primarySourceBufferIDList;

  std::unique_ptr<CompilerStatsReporter> statsReporter;

  mutable ModuleDecl *mainModule = nullptr;
  llvm::SmallVector<IRCodeGenResult *, 8> irCodeGenResults;

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

  void AddIRCodeGenResult(IRCodeGenResult *result) {
    irCodeGenResults.push_back(result);
  }
  void
  ForEachIRCodeGenResult(std::function<void(IRCodeGenResult *result)> notify) {
    for (auto result : irCodeGenResults) {
      notify(result);
    }
  }

public:
  //  bool HasIRCodeGenResult() { return irCodeGenResult != nullptr; }
  //  IRCodeGenResult *GetIRCodeGenResult() { return irCodeGenResult; }

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
  Status ExecuteAction(ActionKind kind);

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
  /// Create the default output file (from the invocation's options) and add it
  /// to the list of tracked output files.
  ///
  /// The files created by this are usually removed on signal, and, depending
  /// on FrontendOptions, may also use a temporary file (that is, the data is
  /// written to a temporary file which will atomically replace the target
  /// output on success).
  ///
  /// \return - Null on error.
  std::unique_ptr<raw_pwrite_stream> CreateDefaultOutputFile(
      bool binary = true, StringRef baseInput = "",
      llvm::StringRef extension = "", bool removeFileOnSignal = true,
      bool createMissingDirectories = false, bool forceUseTemporary = false);

  /// Create a new output file, optionally deriving the output path name, and
  /// add it to the list of tracked output files.
  ///
  /// \return - Null on error.
  std::unique_ptr<raw_pwrite_stream>
  CreateOutputFile(StringRef outputPath, bool binary, bool removeFileOnSignal,
                   bool useTemporary, bool createMissingDirectories = false);

private:
  /// Create a new output file and add it to the list of tracked output files.
  ///
  /// If \p OutputPath is empty, then createOutputFile will derive an output
  /// path location as \p BaseInput, with any suffix removed, and \p Extension
  /// appended. If \p OutputPath is not stdout and \p UseTemporary
  /// is true, createOutputFile will create a new temporary file that must be
  /// renamed to \p OutputPath in the end.
  ///
  /// \param OutputPath - If given, the path to the output file.
  /// \param Binary - The mode to open the file in.
  /// \param RemoveFileOnSignal - Whether the file should be registered with
  /// llvm::sys::RemoveFileOnSignal. Note that this is not safe for
  /// multithreaded use, as the underlying signal mechanism is not reentrant
  /// \param UseTemporary - Create a new temporary file that must be renamed to
  /// OutputPath in the end.
  /// \param CreateMissingDirectories - When \p UseTemporary is true, create
  /// missing directories in the output path.
  Expected<std::unique_ptr<raw_pwrite_stream>>
  PerformCreateOutputFile(StringRef outputPath, bool binary,
                          bool removeFileOnSignal, bool useTemporary,
                          bool createMissingDirectories);

public:
  std::unique_ptr<raw_pwrite_stream> CreateNullOutputFile();

public:
  static Status IsValidModuleName(const llvm::StringRef moduleName);
};

} // namespace stone
#endif
