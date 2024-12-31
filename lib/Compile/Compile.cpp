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
                  return stone::CompletedSemanticAnalysis(instance);
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

  // FrontendStatsTracer actionTracer(instance.GetStats(),
  //                                  GetSelfActionKindString());

  llvm::outs() << "PerformParse" << '\n';

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
  llvm::outs() << "PerformEmitParse" << '\n';
}

// \return true if syntax analysis is successful
bool stone::PerformResolveImports(CompilerInstance &instance,
                                  PerformResolveImportsCallback callback) {
  llvm::outs() << "PerformResolveImports" << '\n';
  return callback(instance);
}

// \return true if semantic analysis is successful
bool stone::PerformSemanticAnalysis(CompilerInstance &instance,
                                    PerformSemanticAnalysisCallback callback) {
  llvm::outs() << "PerformSemanticAnalysis" << '\n';

  return callback(instance);
}

// \return true if emit-ast is true
bool stone::PerformEmitAST(CompilerInstance &instance) {
  llvm::outs() << "PerformEmitParse" << '\n';
}

// \return true if the code generation was successfull
bool stone::CompletedSemanticAnalysis(CompilerInstance &instance) {
  llvm::outs() << "CompletedSemanticAnalysis" << '\n';
}

/// \retyrb true if we compiled an ir file.
bool stone::PerformCompileLLVM(CompilerInstance &instance) {}
