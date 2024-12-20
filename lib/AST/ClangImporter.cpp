#include "stone/AST/ClangImporter.h"

#include "clang/Basic/Stack.h"
#include "clang/Basic/TargetOptions.h"
#include "clang/CodeGen/ObjectFilePCHContainerOperations.h"
#include "clang/Config/config.h"
#include "clang/Driver/DriverDiagnostic.h"
#include "clang/Driver/Options.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/CompilerInvocation.h"
#include "clang/Frontend/FrontendDiagnostic.h"
#include "clang/Frontend/TextDiagnosticBuffer.h"
#include "clang/Frontend/TextDiagnosticPrinter.h"
#include "clang/Frontend/Utils.h"

#include "llvm/ADT/IntrusiveRefCntPtr.h"
#include "llvm/Support/BuryPointer.h"
#include "llvm/Support/CrashRecoveryContext.h"
#include "llvm/Support/Errc.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/LockFileManager.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/Program.h"
#include "llvm/Support/Signals.h"
#include "llvm/Support/TimeProfiler.h"
#include "llvm/Support/Timer.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/TargetParser/Host.h"

using namespace stone;

ClangModuleImporter::ClangModuleImporter() {}

ClangImporter::ClangImporter()
    : clangInstance(new clang::CompilerInstance()),
      fileMgr(GetFileSystemOptions()) {}

bool ClangImporter::Setup(llvm::ArrayRef<const char *> argv, const char *arg0) {

  // Load the diagnostic IDs
  llvm::IntrusiveRefCntPtr<clang::DiagnosticIDs> DiagIDs(
      new clang::DiagnosticIDs());

  // Create options
  llvm::IntrusiveRefCntPtr<clang::DiagnosticOptions> DiagOpts =
      new clang::DiagnosticOptions();

  clang::TextDiagnosticBuffer *DiagBuffer = new clang::TextDiagnosticBuffer;
  clang::DiagnosticsEngine Diags(DiagIDs, &*DiagOpts, DiagBuffer);

  bool Success = clang::CompilerInvocation::CreateFromArgs(
      clangInstance->getInvocation(), argv, Diags, arg0);
  if (!Success) {
    return false;
  }

  // Create the actual diagnostics engine.
  clangInstance->createDiagnostics();
  if (!clangInstance->hasDiagnostics()) {
    return false;
  }

  DiagBuffer->FlushDiagnostics(GetClangInstance().getDiagnostics());
  if (!Success) {
    clangInstance->getDiagnosticClient().finish();
    return false;
  }
  // If there were errors in processing arguments, don't do anything else.
  if (clangInstance->getDiagnostics().hasErrorOccurred()) {
    return false;
  }
  // Set up the file and source managers, if needed.
  if (!clangInstance->hasFileManager()) {
    assert(GetClangInstance().createFileManager());
  }
  if (!clangInstance->hasSourceManager()) {
    clangInstance->createSourceManager(clangInstance->getFileManager());
  }

  assert(clangInstance->createTarget());

  clangInstance->createPreprocessor(clang::TU_Complete);
  clangInstance->createASTContext();
  clangInstance->createASTReader();

  // TODO: Create code generator

  return true;
}

std::unique_ptr<clang::CodeGenerator>
ClangImporter::CreateCodeGenerator(llvm::LLVMContext &llvmContext,
                                   llvm::StringRef moduleName) {

  auto &clangASTContext = GetClangInstance().getASTContext();
  auto &clangCodeGenOpts = GetClangInstance().getCodeGenOpts();

  // TODO: clangCodeGenOpts.OptimizationLevel =
  //     irGen.GetCodeGenOptions().ShouldOptimize() ? 3 : 0;

  clangCodeGenOpts.OptimizationLevel = 0;
  clangCodeGenOpts.CoverageMapping = false;

  auto &vfs = GetClangInstance().getVirtualFileSystem();
  auto &headerSearchOpts = GetClangInstance()
                               .getPreprocessor()
                               .getHeaderSearchInfo()
                               .getHeaderSearchOpts();

  auto &preprocessorOpts =
      GetClangInstance().getPreprocessor().getPreprocessorOpts();

  auto *clangCodeGen = clang::CreateLLVMCodeGen(
      GetClangInstance().getDiagnostics(), moduleName, &vfs, headerSearchOpts,
      preprocessorOpts, clangCodeGenOpts, llvmContext);

  clangCodeGen->Initialize(clangASTContext);

  std::unique_ptr<clang::CodeGenerator> result;
  result.reset(clangCodeGen);

  return result;
}