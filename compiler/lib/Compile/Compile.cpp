#include "stone/AST/ASTDiagnosticArgument.h"
#include "stone/AST/Module.h"
#include "stone/Basic/Defer.h"
#include "stone/Basic/LLVMInit.h"
#include "stone/Basic/MainExecutablePath.h"
#include "stone/CodeCompletionListener.h"
#include "stone/Compile/CompilerInstance.h"
#include "stone/Compile/CompilerInvocation.h"
#include "stone/Diag/CompilerDiagnostic.h"
#include "stone/Diag/TextDiagnosticFormatter.h"
#include "stone/Diag/TextDiagnosticListener.h"
#include "stone/Lang.h"
#include "stone/Options/ModeKind.h"

#include "clang/Basic/TargetInfo.h"

#include "llvm/IR/Module.h"
#include "llvm/Support/Casting.h"

using namespace stone;
using namespace stone::ast;

static bool PerformCompileAfterCodeAnalysis(CompilerInstance &compiler) {}

int Lang::Compile(llvm::ArrayRef<const char *> args, const char *arg0,
                  void *mainAddr, CompilerListener *listener) {

  llvm::PrettyStackTraceString crashInfo("Compile construction...");
  FINISH_LLVM_INIT();

  auto Finish = [&](Status status = Status::Success()) -> int {
    return status.GetFlag();
  };

  auto programPath = llvm::sys::fs::getMainExecutable(arg0, mainAddr);
  auto programName = file::GetStem(programPath);

  CompilerInvocation invocation(programName, programPath, listener);
  STONE_DEFER { invocation.Finish(); };

  if (args.empty()) {
    invocation.GetDiagnoticEngine().PrintD(SrcLoc(), diag::err_no_input_files);
    return Finish(Status::Error());
  }
  // We setup clang now -- this just loads the instance.
  if (invocation.SetupClang(args, arg0).Has()) {

    return Finish(Status::Error());
  }

  // Setup the custom formatting to be able to handle asttax diagnostics
  ASTDiagnosticFormatter diagFormatter;
  ASTDiagnosticEmitter diagEmitter(diagFormatter);
  TextDiagnosticListener diagListener(diagEmitter);

  invocation.GetDiagEngine().AddListener(diagListener);

  ConfigurationFileBuffers configurationFileBuffers;

  // Parse arguments.
  auto status = invocation.ParseArgs(args);
  if (status.IsError()) {
    return Finish(Status::Error());
  }
  if (invocation.HasError()) {
    return Finish(Status::Error());
  }

  if (invocation.IsAlien()) {
    invocation.GetDiagnoticEngine().PrintD(SrcLoc(), diag::err_alien_mode);
    Finish(Status::Error());
  }
  if (invocation.IsPrintHelp()) {
    // TODO: invocation.PrintHelp(invocation.GetOpts());
    return Finish();
  }
  if (invocation.IsPrintVersion()) {
    invocation.PrintVersion();
    return Finish();
  }
  if (!invocation.CanCompile()) {
    /// invocation.PrintD()
    return Finish();
  }
  invocation.NotifyCompileConfigure();

  if (invocation.CreateSourceBuffers().IsError()) {
    return Finish();
  }

  CompilerInstance compilerInstance(invocation);
  auto status = compilerInstance.Compile();

  if (status.IsError() || invocation.HasError()) {
    return Finish(Status::Error());
  }
  return Finish();
}

Status CompilerInstance::Compile() {

  assert(CanCompile() && "Unknown mode -- cannot continue with compile!");
  llvm::TimeTraceScope compileTimeScope("Compile");

  NotifyCompileStarted();

  Lang::CompileWithCodeAnalysis(*this);

  PerformCompileAfterCodeAnalysis(*this);

  Lang::CompileWithCodeGeneration(*this);

  NotifyCompileFinished();

  // CompilingSession compillingSession();
  // compillingSession.NotifyCompileStarted(*this);

  // // At this point, everything requires syntax analysis.
  // if (GetMode().IsParse()) {
  //   if (compillingSession.PerformSyntaxAnalysis(*this).IsError()) {
  //     return Status::Error();
  //   }
  // }

  // // Otherwise, default to performing syntax analysis with import resoltuion.
  // if (compillingSession.PerformSyntaxAnalysisAndImportResoltuion(*this)
  //         .IsError()) {
  //   return Status::Error();
  // }

  // if (GetMode().IsResolveImports()) {
  //   return Status::Success();
  // }

  // // Are we trying to dump the AST?
  // if (GetMode().IsDumpAST()) {
  //   if (compillingSession.PerformDumpAST(*this).IsError()) {
  //     return Status::Error();
  //   }
  //   return Status();
  // }
  // // At this point, everything requires type-checking
  // if (compillingSession.PerformSemanticAnalysis(*this).IsError()) {
  //   return Status::Error();
  // }

  // if (GetMode().IsTypeCheck()) {
  //   return Status::Success();
  // }
  // // Are we trying to print the AST?
  // if (GetMode().IsPrintAST()) {
  //   if (compillingSession.PerformPrintAST(*this).IsError()) {
  //     return Status::Error();
  //   }
  // }

  // if (compillingSession.PerformCompileAfterCodeAnalysis(*this).IsError()) {
  //   return Status::Error();
  // }
  // compillingSession.NotifyCompileFinished(*this);
}
