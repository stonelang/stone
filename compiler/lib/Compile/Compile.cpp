#include "stone/Compile/Compile.h"
#include "stone/AST/Diagnostics.h"
#include "stone/AST/DiagnosticsCompile.h"
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

using namespace stone;

int stone::Compile(llvm::ArrayRef<const char *> args, const char *arg0,
                   void *mainAddr, CompilerObservation *observation) {

  llvm::PrettyStackTraceString crashInfo("Compile construction...");
  FINISH_LLVM_INIT();

  TextDiagnosticPrinter printer;
  CompilerInvocation invocation;
  invocation.AddDiagnosticConsumer(printer);

  auto FinishCompile = [&](Status status = Status::Success()) -> int {
    return (status.IsError() ? status.GetFlag()
                             : invocation.GetDiags().finishProcessing());
  };

  auto mainExecutablePath = llvm::sys::fs::getMainExecutable(arg0, mainAddr);
  invocation.SetMainExecutablePath(mainExecutablePath);
  assert(invocation.GetCompilerOptions().HasMainExecutablePath() &&
         "Did not find an executable path!");

  auto mainExecutableName = llvm::sys::path::stem(arg0);
  invocation.SetMainExecutableName(mainExecutableName);
  assert(invocation.GetCompilerOptions().HasMainExecutableName() &&
         "Did not find an executable name!");

  auto status = invocation.ParseCommandLine(args);
  if (status.IsError()) {
    return FinishCompile(Status::Error());
  }

  CompilerInstance compiler(invocation);

  if (compiler.HasPrimaryAction()) {
    // compiler.GetDiags().diagnose(diag::error_no_compile_action);
    return FinishCompile(Status::Error());
  }

  switch (compiler.GetPrimaryAction()) {
  case CompilerActionKind::PrintHelp:
  case CompilerActionKind::PrintHelpHidden:
  case CompilerActionKind::PrintVersion:
  case CompilerActionKind::PrintFeature: {
    compiler.ExecuteAction();
    return FinishCompile();
  }
  }
  compiler.SetObservation(observation);
  if (compiler.HasObservation()) {
    compiler.GetObservation()->CompletedCommandLineParsing(compiler);
  }

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

  return FinishCompile();
}

bool stone::PerformCompile(CompilerInstance &instance,
                           CompilerActionKind depActionKind) {

  auto CurrentActionKind = [&]() -> CompilerActionKind {
    if (depActionKind != CompilerActionKind::None) {
      return depActionKind;
    }
    return instance.GetInvocation().GetCompilerOptions().GetPrimaryAction();
  }();

  switch (CurrentActionKind) {
  case CompilerActionKind::Parse: {
    return stone::PerformParse(instance);
  }
  case CompilerActionKind::EmitParse: {
    if (!stone::PerformCompile(instance, CompilerActionKind::Parse)) {
      return false;
    }
    return stone::PerformEmitParse(instance);
  }
  case CompilerActionKind::ResolveImports: {
    if (!stone::PerformCompile(instance, CompilerActionKind::Parse)) {
      return false;
    }
    return stone::PerformResolveImports(instance);
  }
  case CompilerActionKind::TypeCheck: {
    if (!stone::PerformCompile(instance, CompilerActionKind::ResolveImports)) {
      return false;
    }
    return stone::PerformTypeCheck(instance);
  }

  case CompilerActionKind::EmitAST: {
    if (!stone::PerformCompile(instance, CompilerActionKind::TypeCheck)) {
      return false;
    }
    return stone::PerformEmitAST(instance);
  }
  }

  if (!stone::PerformCompile(instance, CompilerActionKind::TypeCheck)) {
    return false;
  }
  return stone::PerformEmitCode(instance);
}

// \return true if the code generation was successfull
bool stone::PerformEmitCode(CompilerInstance &instance) {

  if (instance.IsCompileForWholeModule()) {
    // Perform whole modufle
    const PrimaryFileSpecificPaths psps =
        instance.GetInvocation()
            .GetPrimaryFileSpecificPathsForWholeModuleOptimizationMode();
    return stone::PerformEmitCode(instance, instance.GetMainModule(), psps);
  }

  if (instance.IsCompileForSourceFile()) {
    bool success = false;
    instance.ForEachPrimarySourceFile([&](SourceFile &primarySourceFile) {
      // Get the paths for the primary source file.
      const PrimaryFileSpecificPaths psps =
          instance.GetInvocation().GetPrimaryFileSpecificPathsForSyntaxFile(
              primarySourceFile);

      // Perform post semantic analysis on each primary file.
      success |= stone::PerformEmitCode(
          instance, instance.CastToModuleFile(&primarySourceFile), psps);
    });
    return success;
  }

  return false;
}

bool stone::PerformEmitAST(CompilerInstance &instance) { return false; }

// \return true if the code generation was successfull
bool stone::PerformEmitCode(CompilerInstance &instance,
                            ModuleDeclOrModuleFile moduleOrFile,
                            const PrimaryFileSpecificPaths &sps) {

  llvm::GlobalVariable *globalHash;
  llvm::StringRef outputFilename = sps.outputFilename;

  auto codeGenResult = [&]() -> CodeGenResult {
    if (auto *primarySourceFile = llvm::dyn_cast_or_null<SourceFile>(
            moduleOrFile.dyn_cast<ModuleFile *>())) {

      return stone::PerformCodeGenIR(instance, primarySourceFile,
                                     outputFilename, sps, globalHash);
    }

    std::vector<std::string> parallelOutputFilenames =
        instance.GetInvocation()
            .GetCompilerOptions()
            .inputsAndOutputs.CopyOutputFilenames();

    return stone::PerformCodeGenIR(
        instance, moduleOrFile.dyn_cast<ModuleDecl *>(), outputFilename, sps,
        parallelOutputFilenames, globalHash);
  }();

  if (instance.GetInvocation().GetCompilerOptions().IsEmitIRAction()) {
    return true;
  }

  return stone::PerformCodeGenBackend(
      instance, outputFilename, codeGenResult.GetLLVMModule(), globalHash);
}

// \return llvm::Module if IR generation is successful
CodeGenResult stone::PerformCodeGenIR(CompilerInstance &instance,
                                      SourceFile *primarySourceFile,
                                      llvm::StringRef moduleName,
                                      const PrimaryFileSpecificPaths &sps,
                                      llvm::GlobalVariable *&globalHash) {

  assert(
      primarySourceFile->HasTypeChecked() &&
      "Unable to perform ir-gen on a source-file that was not type-checked.");

  CodeGenContext codeGen(instance.GetInvocation().GetCodeGenOptions(),
                         instance.GetASTContext());

  ModuleNameAndOuptFileName moduleNameAndOuptFileName =
      std::make_pair(moduleName, sps.outputFilename);

  CodeGenModule codeGenModule(codeGen, nullptr, moduleNameAndOuptFileName);
  codeGenModule.EmitSourceFile(*primarySourceFile);

  return CodeGenResult(std::move(codeGen.llvmContext),
                       std::unique_ptr<llvm::Module>{
                           codeGenModule.GetClangCodeGen().ReleaseModule()},
                       std::move(codeGen.llvmTargetMachine));
}

///\return the generated module
CodeGenResult
stone::PerformCodeGenIR(CompilerInstance &instance, ModuleDecl *moduleDecl,
                        llvm::StringRef moduleName,
                        const PrimaryFileSpecificPaths &sps,
                        ArrayRef<std::string> parallelOutputFilenames,
                        llvm::GlobalVariable *&globalHash) {

  CodeGenContext codeGen(instance.GetInvocation().GetCodeGenOptions(),
                         instance.GetASTContext());

  ModuleNameAndOuptFileName moduleNameAndOuptFileName =
      std::make_pair(moduleName, sps.outputFilename);
}

// \return true if syntax analysis is successful
bool stone::PerformCodeGenBackend(CompilerInstance &instance,
                                  llvm::StringRef outputFilename,
                                  llvm::Module *irModule,
                                  llvm::GlobalVariable *&globalHash) {

  return CodeGenBackend::EmitOutputFile(
      instance.GetInvocation().GetCodeGenOptions(), instance.GetASTContext(),
      irModule, outputFilename);
}

bool stone::PerformCompileLLVM(CompilerInstance &compiler) { return false; }

bool CompilerInstance::ParseAction::ExecuteAction() {

  FrontendStatsTracer tracer(instance.GetStats(), "parse-source-file");

  CodeCompletionCallbacks *codeCompletionCallbacks = nullptr;
  if (instance.HasObservation()) {
    codeCompletionCallbacks =
        instance.GetObservation()->GetCodeCompletionCallbacks();
  }

  auto PerformParse = [&](CompilerInstance &instance,
                          SourceFile &sourceFile) -> bool {
    Parser parser(sourceFile, instance.GetASTContext());
    if (!parser.ParseTopLevelDecls()) {
      return false;
    }
    return true;
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
bool CompilerInstance::EmitParseAction::ExecuteAction() { return true; }

bool CompilerInstance::ResolveImportsAction::ExecuteAction() {

  /// Abstract away
  FrontendStatsTracer tracer(instance.GetStats(), "resolve imports");
  auto PeformResolveImports = [&](CompilerInstance &instance,
                                  SourceFile &sourceFile) -> bool {
    return true;
  };

  instance.ForEachSourceFileInMainModule([&](SourceFile &sourceFile) {
    if (!PeformResolveImports(instance, sourceFile)) {
      return false;
    }
  });

  return true;
}
bool CompilerInstance::TypeCheckAction::ExecuteAction() {

  FrontendStatsTracer tracer(instance.GetStats(), "type-check");

  auto PerformTypeChecking = [&](CompilerInstance &instance,
                                 SourceFile &sourceFile) -> bool {
    assert(sourceFile.HasParsed() &&
           "Unable to type-check a source-file that was not parsed.");

    TypeChecker checker(sourceFile);
    if (!checker.TypeCheckTopLevelDecls()) {
      return false;
    }
    return true;
  };

  instance.ForEachSourceFileToTypeCheck([&](SourceFile &sourceFile) {
    if (!PerformTypeChecking(instance, sourceFile)) {
      return false;
    }
    sourceFile.SetTypeCheckedStage();
  });
  return true;
}

bool CompilerInstance::EmitASTAction::ExecuteAction() { return true; }

bool CompilerInstance::EmitIRAction::ExecuteAction() {

  if (instance.IsCompileForWholeModule()) {
    // Perform whole modufle
    const PrimaryFileSpecificPaths psps =
        instance.GetInvocation()
            .GetPrimaryFileSpecificPathsForWholeModuleOptimizationMode();

    std::vector<std::string> parallelOutputFilenames =
        instance.GetInvocation()
            .GetCompilerOptions()
            .inputsAndOutputs.CopyOutputFilenames();
    llvm::StringRef outputFilename = psps.outputFilename;

    CodeGenResult result =
        ExecuteAction(instance.GetMainModule(), outputFilename, psps,
                      parallelOutputFilenames, globalHash);
  }
  if (instance.IsCompileForSourceFile()) {
    instance.ForEachPrimarySourceFile([&](SourceFile &primarySourceFile) {
      // Get the paths for the primary source file.
      const PrimaryFileSpecificPaths psps =
          instance.GetInvocation().GetPrimaryFileSpecificPathsForSyntaxFile(
              primarySourceFile);
      llvm::StringRef outputFilename = psps.outputFilename;

      CodeGenResult result =
          ExecuteAction(primarySourceFile, outputFilename, psps, globalHash);
    });
  }
  return true;
}

CodeGenResult CompilerInstance::EmitIRAction::ExecuteAction(
    SourceFile &sourceFile, llvm::StringRef moduleName,
    const PrimaryFileSpecificPaths &sps, llvm::GlobalVariable *&globalHash) {}

///\return the generated module
CodeGenResult CompilerInstance::EmitIRAction::ExecuteAction(
    ModuleDecl *moduleDecl, llvm::StringRef moduleName,
    const PrimaryFileSpecificPaths &sps,
    ArrayRef<std::string> parallelOutputFilenames,
    llvm::GlobalVariable *&globalHash) {}

bool CompilerInstance::EmitObjectAction::ExecuteAction() { return true; }
