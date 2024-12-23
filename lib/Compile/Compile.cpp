#include "stone/Compile/Compile.h"
// #include "stone/AST/Diagnostics.h"
// #include "stone/AST/DiagnosticsCompile.h"

#include "stone/Basic/About.h"
#include "stone/Basic/Defer.h"
#include "stone/Basic/LLVMInit.h"
#include "stone/CodeGen/CodeGenBackend.h"
#include "stone/CodeGen/CodeGenContext.h"
#include "stone/CodeGen/CodeGenModule.h"
#include "stone/CodeGen/CodeGenResult.h"
#include "stone/Compile/CompilerInstance.h"
#include "stone/Compile/CompilerObservation.h"
#include "stone/Parse/CodeCompletionCallbacks.h"
#include "stone/Parse/Parser.h"
#include "stone/Sem/TypeChecker.h"
#include "stone/Support/Statistics.h"

#include "stone/Diag/DiagnosticEngine.h"
#include "stone/Diag/DiagnosticID.h"
#include "stone/Diag/DiagnosticParseKind.h"
#include "stone/Diag/TextDiagnosticPrinter.h"

#include "stone/Support/Statistics.h"

using namespace stone;

int stone::Compile(llvm::ArrayRef<const char *> args, const char *arg0,
                   void *mainAddr, CompilerObservation *observation) {

  llvm::PrettyStackTraceString crashInfo("Compile construction...");
  FINISH_LLVM_INIT();

  CompilerInvocation invocation;
  std::unique_ptr<diags::TextDiagnosticPrinterImpl> DC(
      new diags::TextDiagnosticPrinterImpl());

  diags::DiagnosticEngine DE(invocation.GetSrcMgr(),
                             invocation.GetDiagnosticOptions());

  DE.AddClient(DC.get());
  DE.Diagnose(diag::error_no_input_files);
  DE.FinishProcessing();

  // auto FinishCompile = [&](Status status = Status::Success()) -> int {
  //   return (status.IsError() ? status.GetFlag()
  //                            : invocation.GetDiags().finishProcessing());
  // };

  // auto mainExecutablePath = llvm::sys::fs::getMainExecutable(arg0, mainAddr);
  // invocation.SetMainExecutablePath(mainExecutablePath);
  // assert(invocation.GetCompilerOptions().HasMainExecutablePath() &&
  //        "Did not find an executable path!");

  // auto mainExecutableName = llvm::sys::path::stem(arg0);
  // invocation.SetMainExecutableName(mainExecutableName);
  // assert(invocation.GetCompilerOptions().HasMainExecutableName() &&
  //        "Did not find an executable name!");

  // auto status = invocation.ParseCommandLine(args);
  // if (status.IsError()) {
  //   return FinishCompile(Status::Error());
  // }

  // CompilerInstance compiler(invocation);

  // if (!compiler.HasPrimaryAction()) {
  //   // compiler.GetDiags().diagnose(diag::error_no_compile_action);
  //   return FinishCompile(Status::Error());
  // }

  // if (compiler.GetInvocation().GetCompilerOptions().IsImmediateAction()) {
  //   compiler.ExecuteAction();
  //   return FinishCompile();
  // }

  // compiler.SetObservation(observation);
  // if (compiler.HasObservation()) {
  //   compiler.GetObservation()->CompletedCommandLineParsing(compiler);
  // }
  // // Now, setup the compiler
  // if (!compiler.Setup()) {
  //   return FinishCompile(Status::Error());
  // }

  // if (compiler.HasObservation()) {
  //   compiler.GetObservation()->CompletedConfiguration(compiler);
  // }
  // if (!compiler.ExecuteAction()) {
  //   return FinishCompile(Status::Error());
  // }
  // return FinishCompile();
}

bool CompilerInstance::PrintHelpAction::ExecuteAction() {
  assert(GetSelfActionKind() == GetPrimaryActionKind() &&
         "PrintHelpAction has to be the PrimaryAction!");

  instance.GetInvocation().GetCompilerOptions().PrintHelp();
  return true;
}
bool CompilerInstance::PrintHelpHiddenAction::ExecuteAction() {
  assert(GetSelfActionKind() == GetPrimaryActionKind() &&
         "PrintHelpHiddenAction has to be the PrimaryAction!");

  instance.GetInvocation().GetCompilerOptions().PrintHelp(true);
  return true;
}
bool CompilerInstance::PrintVersionAction::ExecuteAction() {
  assert(GetSelfActionKind() == GetPrimaryActionKind() &&
         "PrintVersionAction has to be the PrimaryAction!");

  return true;
}

bool CompilerInstance::PrintFeatureAction::ExecuteAction() {
  assert(GetSelfActionKind() == GetPrimaryActionKind() &&
         "PrintFeatureAction has to be the PrimaryAction!");

  return true;
}

bool CompilerInstance::ParseAction::ExecuteAction() {

  FrontendStatsTracer actionTracer(instance.GetStats(),
                                   GetSelfActionKindString());

  CodeCompletionCallbacks *codeCompletionCallbacks = nullptr;
  if (instance.HasObservation()) {
    codeCompletionCallbacks =
        instance.GetObservation()->GetCodeCompletionCallbacks();
  }

  auto PerformParse = [&](CompilerInstance &instance,
                          SourceFile &sourceFile) -> bool {
    return Parser(sourceFile, instance.GetASTContext()).ParseTopLevelDecls();
  };

  instance.ForEachSourceFileInMainModule([&](SourceFile &sourceFile) {
    if (!PerformParse(instance, sourceFile)) {
      return false;
    }
    sourceFile.SetParsedStage();
    if (codeCompletionCallbacks) {
      codeCompletionCallbacks->CompletedParseSourceFile(&sourceFile);
    }
  });

  return true;
}
// bool CompilerInstance::EmitParseAction::ExecuteAction() {
//   FrontendStatsTracer actionTracer(instance.GetStats(),
//                                    GetSelfActionKindString());

//   return true;
// }

// bool CompilerInstance::ResolveImportsAction::ExecuteAction() {

//   FrontendStatsTracer actionTracer(instance.GetStats(),
//                                    GetSelfActionKindString());

//   auto PeformResolveImports = [&](CompilerInstance &instance,
//                                   SourceFile &sourceFile) -> bool {
//     return true;
//   };

//   instance.ForEachSourceFileInMainModule([&](SourceFile &sourceFile) {
//     if (!PeformResolveImports(instance, sourceFile)) {
//       return false;
//     }
//   });

//   return true;
// }
// bool CompilerInstance::TypeCheckAction::ExecuteAction() {

//   FrontendStatsTracer actionTracer(instance.GetStats(),
//                                    GetSelfActionKindString());

//   auto PerformTypeChecking = [&](CompilerInstance &instance,
//                                  SourceFile &sourceFile) -> bool {
//     assert(sourceFile.HasParsed() &&
//            "Unable to type-check a source-file that was not parsed.");

//     return TypeChecker(sourceFile).TypeCheckTopLevelDecls();
//   };

//   instance.ForEachSourceFileToTypeCheck([&](SourceFile &sourceFile) {
//     if (!PerformTypeChecking(instance, sourceFile)) {
//       return false;
//     }
//     sourceFile.SetTypeCheckedStage();
//   });
//   return true;
// }

// bool CompilerInstance::EmitASTAction::ExecuteAction() {
//   FrontendStatsTracer actionTracer(instance.GetStats(),
//                                    GetSelfActionKindString());

//   return true;
// }

// bool CompilerInstance::EmitIRAction::ExecuteAction() {
//   FrontendStatsTracer actionTracer(instance.GetStats(),
//                                    GetSelfActionKindString());

//   auto TryNotifyEmitCodeConsumer = [&](CodeGenResult *result) -> void {
//     if (HasConsumer()) {
//       if (auto emittedCodeConsumer =
//               llvm::cast<EmitCodeAction>(GetConsumer())) {
//         emittedCodeConsumer->ConsumeEmittedCode(result);
//       }
//     }
//   };
//   if (instance.IsCompileForWholeModule()) {
//     // Perform whole modufle
//     const PrimaryFileSpecificPaths psps =
//         instance.GetPrimaryFileSpecificPathsForWholeModuleOptimizationMode();

//     std::vector<std::string> parallelOutputFilenames =
//         instance.GetCopyOfOutputFilenames();

//     llvm::StringRef outputFilename = psps.outputFilename;

//     CodeGenResult result =
//         ExecuteAction(instance.GetMainModule(), outputFilename, psps,
//                       parallelOutputFilenames, globalHash);

//     TryNotifyEmitCodeConsumer(&result);
//   }
//   if (instance.IsCompileForSourceFile()) {
//     instance.ForEachPrimarySourceFile([&](SourceFile &primarySourceFile) {
//       // Get the paths for the primary source file.
//       const PrimaryFileSpecificPaths psps =
//           instance.GetPrimaryFileSpecificPathsForSyntaxFile(primarySourceFile);

//       llvm::StringRef outputFilename = psps.outputFilename;

//       CodeGenResult result =
//           ExecuteAction(primarySourceFile, outputFilename, psps, globalHash);
//       TryNotifyEmitCodeConsumer(&result);
//     });
//   }
//   return true;
// }

// CodeGenResult CompilerInstance::EmitIRAction::ExecuteAction(
//     SourceFile &primarySourceFile, llvm::StringRef moduleName,
//     const PrimaryFileSpecificPaths &sps, llvm::GlobalVariable *&globalHash) {

//   assert(
//       primarySourceFile.HasTypeChecked() &&
//       "Unable to perform ir-gen on a source-file that was not
//       type-checked.");

//   CodeGenContext codeGenContext(instance.GetInvocation().GetCodeGenOptions(),
//                                 instance.GetASTContext());

//   ModuleNameAndOuptFileName moduleNameAndOuptFileName =
//       std::make_pair(moduleName, sps.outputFilename);

//   CodeGenModule codeGenModule(codeGenContext, nullptr,
//                               moduleNameAndOuptFileName);
//   codeGenModule.EmitSourceFile(primarySourceFile);

//   return CodeGenResult(std::move(codeGenContext.llvmContext),
//                        std::unique_ptr<llvm::Module>{
//                            codeGenModule.GetClangCodeGen().ReleaseModule()},
//                        std::move(codeGenContext.llvmTargetMachine),
//                        sps.outputFilename, globalHash);
// }

// ///\return the generated module
// CodeGenResult CompilerInstance::EmitIRAction::ExecuteAction(
//     ModuleDecl *moduleDecl, llvm::StringRef moduleName,
//     const PrimaryFileSpecificPaths &sps,
//     ArrayRef<std::string> parallelOutputFilenames,
//     llvm::GlobalVariable *&globalHash) {}

// bool CompilerInstance::EmitObjectAction::ExecuteAction() {

//   FrontendStatsTracer emitObjectActionTracer(instance.GetStats(),
//                                              GetSelfActionKindString());

//   return true;
// }

// void CompilerInstance::EmitObjectAction::ConsumeEmittedCode(
//     CodeGenResult *result) {

//   CodeGenBackend::EmitOutputFile(
//       instance.GetInvocation().GetCodeGenOptions(), instance.GetASTContext(),
//       result->GetLLVMModule(), result->GetOutputFilename());
// }
