#ifndef STONE_COMPILE_COMPILERINSTANCE_H
#define STONE_COMPILE_COMPILERINSTANCE_H

#include "stone/AST/Diagnostics.h"
#include "stone/Compile/CompilerInputFile.h"
#include "stone/Compile/CompilerInvocation.h"
#include "stone/Support/Statistics.h"

// #include "llvm/Support/VirtualOutputBackend.h"

#include <deque>
#include <functional>
#include <memory>
#include <utility>
#include <vector>

namespace stone {

class ModuleDecl;
class CodeGenResult;
class CompilerAction;
class CompilerObservation;

class CompilerInstance final {
  friend class CompilerAction;

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

  /// Virtual OutputBackend.
  // llvm::IntrusiveRefCntPtr<llvm::vfs::OutputBackend> outputBackend = nullptr;

  CompilerObservation *observation = nullptr;

public:
  CompilerInstance(const CompilerInstance &) = delete;
  void operator=(const CompilerInstance &) = delete;

public:
  CompilerInstance(CompilerInvocation &invocation);
  bool Setup();
  bool ExecuteAction();
  bool ExecuteAction(CompilerActionKind kind);
  bool ExecuteAction(CompilerAction &compilerAction);

private:
  std::unique_ptr<CompilerAction> ConstructAction(CompilerActionKind kind);

public:
  bool HasObservation() { return observation != nullptr; }
  void SetObservation(CompilerObservation *obs) { observation = obs; }
  CompilerObservation *GetObservation() { return observation; }
  CompilerInvocation &GetInvocation() { return invocation; }
  const CompilerInvocation &GetInvocation() const { return invocation; }

public:
  ASTContext &GetASTContext() { return *astContext; }
  const ASTContext &GetASTContext() const { return *astContext; }
  bool HasASTContext() const { return astContext != nullptr; }

  StatsReporter *GetStats() { return stats.get(); }

  ///\the primary requested action
  CompilerActionKind GetPrimaryActionKind() const {
    return invocation.GetCompilerOptions().GetPrimaryAction();
  }

  bool HasPrimaryAction() const {
    return invocation.GetCompilerOptions().HasPrimaryAction();
  }

public:
  ///\create the source files from the compiler input
  Status SetupCompilerInputFiles();

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

public:
  ///\returns the main module - if it is not created, create it.
  ModuleDecl *GetMainModule() const;

  /// Set the main module if we have not already created it
  void SetMainModule(ModuleDecl *mainModule);

  ///\return the created source file for this module.
  Status CreateSourceFilesForMainModule(
      ModuleDecl *mod, llvm::SmallVectorImpl<ModuleFile *> &files) const;

  ///\return the created source file for this mould.
  SourceFile *CreateSourceFileForMainModule(ModuleDecl *mainModule,
                                            SourceFileKind fileKind,
                                            unsigned bufferID,
                                            bool isMainBuffer = false) const;

  ///\return the main source file for this module
  SourceFile *ComputeMainSourceFileForModule(ModuleDecl *mod) const;

public:
  ///\return all of the source files in the module -- return true if successful
  bool ForEachSourceFileInMainModule(
      llvm::function_ref<bool(SourceFile &sourceFile)> notify);

  ///\return every primary file
  bool ForEachPrimarySourceFile(
      llvm::function_ref<bool(SourceFile &sourceFile)> notify) const;

  /// all of the files to typecheck
  bool ForEachSourceFileToTypeCheck(
      llvm::function_ref<bool(SourceFile &sourceFile)> notify);

  ///\return true if the compile is for the entire module.
  bool IsWholeModuleCompilation() const {
    return primarySourceBufferIDList.empty();
  }

  ///\return true if there are primary CompilerInputFile(s)
  bool HasPrimaryInputFiles() const {
    return invocation.GetCompilerOptions().inputsAndOutputs.HasPrimaryInputs();
  }

  bool IsCompileForWholeModule() { return !HasPrimaryInputFiles(); }
  ///\return true if there are primary files
  bool IsCompileForSourceFile() { HasPrimarySourceFiles(); }

  /// Gets the set of SourceFiles which are the primary inputs for this
  /// CompilerInstance.
  llvm::ArrayRef<SourceFile *> GetPrimarySourceFiles() const {
    return GetMainModule()->GetPrimarySourceFiles();
  }
  ///\return true if there are primary SourceFile(s)
  bool HasPrimarySourceFiles() const {
    return GetPrimarySourceFiles().size() > 0;
  }

  ModuleFile *CastToModuleFile(ModuleDeclOrModuleFile moduleOrFile) {
    return llvm::dyn_cast_or_null<SourceFile>(
        moduleOrFile.dyn_cast<ModuleFile *>());
  }

  ///\return true if this file in an ir file.
  bool IsCompileLLVM() const {
    return GetInvocation()
        .GetCompilerOptions()
        .inputsAndOutputs.ShouldTreatAsLLVM();
  }

  std::vector<std::string> GetCopyOfOutputFilenames() const {
    return invocation.GetCompilerOptions()
        .inputsAndOutputs.CopyOutputFilenames();
  }

  const PrimaryFileSpecificPaths
  GetPrimaryFileSpecificPathsForWholeModuleOptimizationMode() const {
    return invocation
        .GetPrimaryFileSpecificPathsForWholeModuleOptimizationMode();
  }

  const PrimaryFileSpecificPaths
  GetPrimaryFileSpecificPathsForSyntaxFile(SourceFile &primarySourceFile) {
    return invocation.GetPrimaryFileSpecificPathsForSyntaxFile(
        primarySourceFile);
  }

public:
  /// Try to free the ast context
  void TryFreeASTContext();

  /// Free the AST context
  void FreeASTContext();

  ///\return true if we should setup the ASTContext
  bool ShouldSetupASTContext();

  /// Setup the ASTContext
  bool SetupASTContext();

  /// Try to load the SDT libs.
  bool TryLoadSTDLib();

  /// Setup the statistics
  void SetupStats();

public:
  ///\return anf error f we cannot create dir -- move somewhere else.
  std::error_code CreateDirectory(std::string name);
};

} // namespace stone
#endif