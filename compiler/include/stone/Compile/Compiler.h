#ifndef STONE_COMPILE_COMPILER_H
#define STONE_COMPILE_COMPILER_H

#include "stone/Compile/CompilerInputFile.h"
#include "stone/Compile/CompilerInvocation.h"
#include "stone/Support/DiagnosticEngine.h"
#include "stone/Support/Statistics.h"
//
// #include "llvm/Support/HashingOutputBackend.h"
// #include "llvm/Support/VirtualOutputBackend.h"

#include <deque>

namespace stone {

class ModuleDecl;
class IRGenResult;
class CompilerObservation;
class CompilerExecution;

class Compiler final {

  CompilerInvocation &invocation;

  std::unique_ptr<ASTContext> astContext;

  /// If there is no stats output directory by the time the
  /// instance has completed its setup, this will be null.
  std::unique_ptr<StatsReporter> stats;

  /// Contains buffer IDs for input source code files.
  std::vector<unsigned> inputSourceBufferIDList;

  /// The primary Sources
  llvm::SetVector<unsigned> primarySourceBufferIDList;

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
  Compiler(CompilerInvocation &invocation);
  Status Setup();

  Status ExecuteAction(CompilerAction action);
  Status ExecuteAction(CompilerExecution &execution);

public:
  bool HasObservation() { return observation != nullptr; }
  void SetObservation(CompilerObservation *obs) { observation = obs; }
  CompilerObservation *GetObservation() { return observation; }

  void AddIRGenResult(IRGenResult *result) { irGenResults.push_back(result); }
  void ForEachIRGenResult(std::function<void(IRGenResult *result)> notify) {
    for (auto result : irGenResults) {
      notify(result);
    }
  }

public:
  ASTContext &GetASTContext() { return *astContext; }
  const ASTContext &GetASTContext() const { return *astContext; }
  bool HasASTContext() const { return astContext != nullptr; }

  CompilerInvocation &GetInvocation() { return invocation; }

  CompilerAction GetMainAction() const {
    return invocation.GetCompilerOptions().GetMainAction();
  }

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

  std::optional<unsigned> CreateCodeCompletionBuffer();
  std::optional<unsigned> GetRecordedBufferID(const CompilerInputFile &input,
                                              const bool shouldRecover,
                                              bool &failed);
  std::optional<ModuleBuffers>
  GetInputBuffersIfPresent(const CompilerInputFile &input);

  SourceFile::ParsingOptions GetSourceFileParsingOptions(bool forPrimary) const;

  std::error_code CreateDirectory(std::string name) {
    return llvm::sys::fs::create_directories(name);
  }

  std::unique_ptr<CompilerExecution> CreateExectution(CompilerAction action);

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

  bool IsWholeModuleCompile() {
    if (IsCompileForWholeModule()) {
      return true;
    } else if (IsCompileForSourceFile()) {
      return false;
    } else {
      llvm_unreachable("Invalid IR code generation target!");
    }
  }

  bool IsLLVMCompile() {
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
  bool TryLoadSTDLib();

  void SetupStats();
  StatsReporter *GetStats() { return stats.get(); }

public:
  static Status IsValidModuleName(const llvm::StringRef moduleName);

public:
  void PrintHelp(bool showHidden = false) const;
};

} // namespace stone
#endif