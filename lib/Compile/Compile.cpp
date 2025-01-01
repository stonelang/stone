#include "stone/Compile/Compile.h"
#include "stone/AST/Diagnostics.h"
#include "stone/AST/DiagnosticsCompile.h"
#include "stone/AST/Module.h"
#include "stone/AST/TypeChecker.h"
#include "stone/Basic/About.h"
#include "stone/Basic/Defer.h"
#include "stone/Basic/LLVMInit.h"
#include "stone/CodeGen/CodeGenBackend.h"
#include "stone/CodeGen/CodeGenContext.h"
#include "stone/CodeGen/CodeGenModule.h"
#include "stone/Compile/Compile.h"
#include "stone/Compile/CompilerAction.h"
#include "stone/Compile/CompilerInstance.h"
#include "stone/Compile/CompilerObservation.h"
#include "stone/Parse/CodeCompletionCallbacks.h"
#include "stone/Parse/Parser.h"
#include "stone/Support/Statistics.h"

using namespace stone;

int stone::Compile(llvm::ArrayRef<const char *> args, const char *arg0,
                   void *mainAddr, CompilerObservation *observation) {

  llvm::PrettyStackTraceString crashInfo("Compile construction...");
  FINISH_LLVM_INIT();

  CompilerInvocation invocation;
  CompilerDiagnosticPrinter printer(invocation.GetSrcMgr());
  invocation.AddDiagnosticConsumer(printer);

  auto FinishCompile = [&](Status status = Status::Success()) -> int {
    if (invocation.GetDiags().finishProcessing()) {
      return 1;
    }
    if (status.IsError()) {
      return 1;
    }
    return 0;
  };

  auto mainExecutablePath = llvm::sys::fs::getMainExecutable(arg0, mainAddr);
  invocation.SetMainExecutablePath(mainExecutablePath);
  assert(invocation.GetCompilerOptions().HasMainExecutablePath() &&
         "Did not find an executable path!");

  auto mainExecutableName = llvm::sys::path::stem(arg0);
  invocation.SetMainExecutableName(mainExecutableName);
  assert(invocation.GetCompilerOptions().HasMainExecutableName() &&
         "Did not find an executable name!");

  if (args.empty()) {
    invocation.GetDiags().diagnose(SrcLoc(), diag::error_no_input_files);
    return FinishCompile(Status::Error());
  }

  auto status = invocation.ParseArgs(args);
  if (status.IsError()) {
    return FinishCompile(Status::Error());
  }
  printer.setFormattingStyle(invocation.GetDiagnosticOptions().formattingStyle);

  CompilerInstance instance(invocation);

  if (!instance.HasPrimaryAction()) {
    // instance.GetDiags().diagnose(diag::error_no_compile_action);
    return FinishCompile(Status::Error());
  }

  switch (instance.GetPrimaryActionKind()) {
  case CompilerActionKind::PrintHelp: {
    instance.GetInvocation().GetCompilerOptions().PrintHelp();
    return FinishCompile();
  }
  case CompilerActionKind::PrintHelpHidden: {
    instance.GetInvocation().GetCompilerOptions().PrintHelp(true);
    return FinishCompile();
  }
  case CompilerActionKind::PrintVersion: {
    stone::PrintCompilerVersion();
    return FinishCompile();
  }
  case CompilerActionKind::PrintFeature: {
    stone::PrintCompilerFeatures();
    return FinishCompile();
  }
  default: {
    break;
  }
  }

  instance.SetObservation(observation);
  if (instance.HasObservation()) {
    instance.GetObservation()->CompletedCommandLineParsing(instance);
  }
  // Now, setup the instance
  if (!instance.Setup()) {
    return FinishCompile(Status::Error());
  }

  if (instance.HasObservation()) {
    instance.GetObservation()->CompletedConfiguration(instance);
  }
  if (!PerformCompile(instance)) {
    return FinishCompile(false);
  }
  return FinishCompile();
}

bool stone::PerformCompile(CompilerInstance &instance) {
  return PerformAction(instance);
}
/// \return true if compilie is successful
bool stone::PerformAction(CompilerInstance &instance) {
  switch (instance.GetPrimaryActionKind()) {
  case CompilerActionKind::Parse: {
    return stone::PerformParse(instance, [](CompilerInstance &instance) {
      return instance.GetInvocation().HasError();
    });
  }
  case CompilerActionKind::EmitParse: {
    return stone::PerformParse(instance, [&](CompilerInstance &instance) {
      return stone::PerformEmitParse(instance);
    });
  }
  case CompilerActionKind::ResolveImports: {
    return stone::PerformParse(instance, [&](CompilerInstance &instance) {
      return stone::PerformResolveImports(instance);
    });
  }
  case CompilerActionKind::TypeCheck: {
    return stone::PerformParse(instance, [&](CompilerInstance &instance) {
      return stone::PerformResolveImports(
          instance, [&](CompilerInstance &instance) {
            return stone::PerformSemanticAnalysis(instance);
          });
    });
  }
  case CompilerActionKind::EmitAST: {
    return stone::PerformParse(instance, [&](CompilerInstance &instance) {
      return stone::PerformResolveImports(
          instance, [&](CompilerInstance &instance) {
            return stone::PerformSemanticAnalysis(
                instance, [&](CompilerInstance &instance) {
                  return PerformEmitAST(instance);
                });
          });
    });
  }
  case CompilerActionKind::EmitIR:
  case CompilerActionKind::EmitBC:
  case CompilerActionKind::EmitModule:
  case CompilerActionKind::EmitObject:
  case CompilerActionKind::MergeModules:
  case CompilerActionKind::EmitAssembly: {
    return stone::PerformParse(instance, [&](CompilerInstance &instance) {
      return stone::PerformResolveImports(
          instance, [&](CompilerInstance &instance) {
            return stone::PerformSemanticAnalysis(
                instance, [&](CompilerInstance &instance) {
                  return stone::PerformEmitCode(instance);
                });
          });
    });
  }
  default: {
    break;
  }
  }
}

/// \return true if syntax analysis is successful
bool stone::PerformParse(CompilerInstance &instance,
                         PerformParseCallback callback) {

  FrontendStatsTracer frontendTracer(
      instance.GetStats(), instance.GetActionString(CompilerActionKind::Parse));
  auto CompletedParseSourceFile = [&](CompilerInstance &instance,
                                      SourceFile &sourceFile) -> void {
    if (instance.HasObservation()) {
      auto codeCompletionCallbacks =
          instance.GetObservation()->GetCodeCompletionCallbacks();
      if (codeCompletionCallbacks) {
        codeCompletionCallbacks->CompletedParseSourceFile(&sourceFile);
      }
    }
  };
  instance.ForEachSourceFileInMainModule([&](SourceFile &sourceFile) {
    if (!Parser(sourceFile, instance.GetASTContext()).ParseTopLevelDecls()) {
      return false;
    }
    sourceFile.SetParsedStage();
    CompletedParseSourceFile(instance, sourceFile);
    return true;
  });
  return callback(instance);
}

/// \return true if syntax analysis is successful for a specific source file
bool stone::PerformEmitParse(CompilerInstance &instance) {
  FrontendStatsTracer frontendTracer(
      instance.GetStats(),
      instance.GetActionString(CompilerActionKind::EmitParse));
}

// \return true if syntax analysis is successful
bool stone::PerformResolveImports(CompilerInstance &instance,
                                  PerformResolveImportsCallback callback) {
  FrontendStatsTracer frontendTracer(
      instance.GetStats(),
      instance.GetActionString(CompilerActionKind::ResolveImports));

  return callback(instance);
}

// \return true if semantic analysis is successful
bool stone::PerformSemanticAnalysis(CompilerInstance &instance,
                                    PerformSemanticAnalysisCallback callback) {
  FrontendStatsTracer frontendTracer(
      instance.GetStats(),
      instance.GetActionString(CompilerActionKind::TypeCheck));

  return callback(instance);
}

// \return true if emit-ast is true
bool stone::PerformEmitAST(CompilerInstance &instance) {
  FrontendStatsTracer frontendTracer(
      instance.GetStats(),
      instance.GetActionString(CompilerActionKind::EmitAST));
}

// \return true if the code generation was successfull
bool stone::PerformEmitCode(CompilerInstance &instance) {

  switch (instance.GetPrimaryActionKind()) {
  case CompilerActionKind::EmitIR: {

    return stone::PerformEmitIR(
        instance, [&](CompilerInstance &instance, CodeGenResult &result) {
          return instance.GetInvocation().HasError();
        });
  }
  // case CompilerActionKind::EmitObject: {
  //   return stone::PerformEmitIR(
  //       instance, [](CompilerInstance &instance, CodeGenResult &result) {
  //         return stone::PerformEmitBackend(instance, result);
  //       });
  // }
  default: {
  }
  }
}

bool stone::PerformEmitIR(CompilerInstance &instance,
                          PerformEmitIRCallback callback) {

  llvm::GlobalVariable *globalHash;
  if (instance.IsCompileForWholeModule()) {
    // Perform whole modufle
    // const PrimaryFileSpecificPaths psps =
    //     instance.GetPrimaryFileSpecificPathsForWholeModuleOptimizationMode();

    // std::vector<std::string> parallelOutputFilenames =
    //     instance.GetCopyOfOutputFilenames();

    // llvm::StringRef outputFilename = psps.outputFilename;

    // CodeGenResult result = stone::PerformEmitModule(
    //     instance, instance.GetMainModule(), outputFilename, psps,
    //     parallelOutputFilenames, globalHash);

    // if (!result) {
    //   return false;
    // } else {
    //   return callback(instance, result);
    // }

  } else if (instance.IsCompileForSourceFile()) {
    instance.ForEachPrimarySourceFile([&](SourceFile &primarySourceFile) {
      // Get the paths for the primary source file.
      const PrimaryFileSpecificPaths psps =
          instance.GetPrimaryFileSpecificPathsForSyntaxFile(primarySourceFile);

      llvm::StringRef outputFilename = psps.outputFilename;
      CodeGenResult result = stone::PerformEmitSourceFile(
          instance, primarySourceFile, outputFilename, psps, globalHash);

      if (!result) {
        return false;
      } else {
        return callback(instance, result);
      }
    });
  }
  assert(false && "invalid call to PerformEmitIR");
}

// // \return true if the code generation was successfull
// bool stone::PerformEmitCodeImpl(CompilerInstance &instance,
//                                 ModuleDeclOrModuleFile moduleOrFile,
//                                 const PrimaryFileSpecificPaths &sps) {

//   auto PerformCodeGenIR =
//       [&](CompilerInstance &instance, ModuleDeclOrModuleFile moduleOrFile,
//           const PrimaryFileSpecificPaths &sps) -> CodeGenResult {

//     if (auto *SF = moduleOrFile.dyn_cast<SourceFile *>()) {
//     } else {

//       auto *MD = moduleOrFile.get<ModuleDecl *>();
//     }
//   };

//   // auto *SF = moduleOrFile.dyn_cast<SourceFile *>();

//   // auto  *MD = moduleOrFile.get<ModuleDecl *>();
// }

// // \return llvm::Module if IR generation is successful
CodeGenResult stone::PerformEmitSourceFile(CompilerInstance &instance,
                                           SourceFile &sourceFile,
                                           llvm::StringRef moduleName,
                                           const PrimaryFileSpecificPaths &sps,
                                           llvm::GlobalVariable *&globalHash) {}

// ///\return the generated module
CodeGenResult
stone::PerformEmitModule(CompilerInstance &instance, ModuleDecl *moduleDecl,
                         llvm::StringRef moduleName,
                         const PrimaryFileSpecificPaths &sps,
                         ArrayRef<std::string> parallelOutputFilenames,
                         llvm::GlobalVariable *&globalHash) {}

bool stone::PerformEmitBackend(CompilerInstance &instance,
                               llvm::StringRef outputFilename,
                               llvm::Module *module,
                               llvm::GlobalVariable *&globalHash) {}
/// \retyrb true if we compiled an ir file.
bool stone::PerformCompileLLVM(CompilerInstance &instance) {}
