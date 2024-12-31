#include "stone/AST/TypeChecker.h"
#include "stone/CodeGen/CodeGenBackend.h"
#include "stone/CodeGen/CodeGenContext.h"
#include "stone/CodeGen/CodeGenModule.h"
#include "stone/Compile/CompilerAction.h"
#include "stone/Compile/CompilerInstance.h"

bool EmitCodeAction::ExecuteAction() {}

void EmitCodeAction::AddCodeGenResult(CodeGenResult &&result) {
  CodeGenResults.push_back(std::move(result));
}

bool EmitIRAction::ExecuteAction() {
  FrontendStatsTracer actionTracer(instance.GetStats(),
                                   GetSelfActionKindString());

  /// Execute any requirements before executing emit-ir
  EmitCodeAction::ExecuteAction();

  auto NotifyCodeGenConsumer = [&](CodeGenResult *result) -> void {
    if (HasConsumer()) {
      if (auto codeGenConsumer = llvm::cast<EmitCodeAction>(GetConsumer())) {
        codeGenConsumer->ConsumeCodeGen(result);
      }
    }
  };
  if (instance.IsCompileForWholeModule()) {
    // Perform whole modufle
    const PrimaryFileSpecificPaths psps =
        instance.GetPrimaryFileSpecificPathsForWholeModuleOptimizationMode();

    std::vector<std::string> parallelOutputFilenames =
        instance.GetCopyOfOutputFilenames();

    llvm::StringRef outputFilename = psps.outputFilename;

    CodeGenResult result =
        EmitModuleDecl(instance.GetMainModule(), outputFilename, psps,
                       parallelOutputFilenames, globalHash);

    NotifyCodeGenConsumer(&result);
    AddCodeGenResult(std::move(result));
  }
  if (instance.IsCompileForSourceFile()) {

    instance.ForEachPrimarySourceFile([&](SourceFile &primarySourceFile) {
      // Get the paths for the primary source file.
      const PrimaryFileSpecificPaths psps =
          instance.GetPrimaryFileSpecificPathsForSyntaxFile(primarySourceFile);

      llvm::StringRef outputFilename = psps.outputFilename;

      CodeGenResult result =
          EmitSourceFile(primarySourceFile, outputFilename, psps, globalHash);

      if (!result) {
        return false;
      }

      NotifyCodeGenConsumer(&result);
      AddCodeGenResult(std::move(result));
    });
  }
  if (HasConsumer()) {
    GetConsumer()->DepCompleted(this);
  }

  if (ShouldOutput()) {
  }
  return true;
}

CodeGenResult EmitIRAction::EmitSourceFile(SourceFile &primarySourceFile,
                                           llvm::StringRef moduleName,
                                           const PrimaryFileSpecificPaths &sps,
                                           llvm::GlobalVariable *&globalHash) {

  assert(
      primarySourceFile.HasTypeChecked() &&
      "Unable to perform ir-gen on a source-file that was not type-checked!");

  CodeGenContext codeGenContext(instance.GetInvocation().GetCodeGenOptions(),
                                instance.GetASTContext());

  ModuleNameAndOuptFileName moduleNameAndOuptFileName =
      std::make_pair(moduleName, sps.outputFilename);

  CodeGenModule codeGenModule(codeGenContext, nullptr,
                              moduleNameAndOuptFileName);
  codeGenModule.EmitSourceFile(primarySourceFile);

  return CodeGenResult(std::move(codeGenContext.llvmContext),
                       std::unique_ptr<llvm::Module>{
                           codeGenModule.GetClangCodeGen().ReleaseModule()},
                       std::move(codeGenContext.llvmTargetMachine),
                       sps.outputFilename, globalHash);
}

///\return the generated module
CodeGenResult
EmitIRAction::EmitModuleDecl(ModuleDecl *moduleDecl, llvm::StringRef moduleName,
                             const PrimaryFileSpecificPaths &sps,
                             ArrayRef<std::string> parallelOutputFilenames,
                             llvm::GlobalVariable *&globalHash) {}

bool EmitObjectAction::ExecuteAction() {

  FrontendStatsTracer emitObjectActionTracer(instance.GetStats(),
                                             GetSelfActionKindString());
  return true;
}

void EmitObjectAction::ConsumeCodeGen(CodeGenResult *result) {

  CodeGenBackend::EmitOutputFile(
      instance.GetInvocation().GetCodeGenOptions(), instance.GetASTContext(),
      result->GetLLVMModule(), result->GetOutputFilename());
}

void EmitObjectAction::DepCompleted(CompilerAction *dep) {
  if (dep) {
    assert((GetDepActionKind() == dep->GetSelfActionKind()) &&
           "EmitObjectAction did not call the dependency!");
    if (auto emitCodeAction = llvm::cast<EmitCodeAction>(dep)) {
    }
  }
}
