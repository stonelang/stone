#include "stone/Compile/Compile.h"
#include "stone/Basic/Defer.h"
#include "stone/Basic/LLVMInit.h"
#include "stone/Compile/Compiler.h"
#include "stone/Compile/CompilerExecution.h"
#include "stone/Option/ActionKind.h"
#include "stone/Public.h"

#include "stone/Diag/CompilerDiagnostic.h"
#include "stone/Diag/TextDiagnosticConsumer.h"
#include "stone/Diag/TextDiagnosticFormatter.h"
#include "stone/Syntax/ASTDiagnosticArgument.h"

using namespace stone;

int stone::Compile(llvm::ArrayRef<const char *> args, const char *arg0,
                   void *mainAddr, CompilerListener *listener) {

  llvm::PrettyStackTraceString crashInfo("Compile construction...");
  FINISH_LLVM_INIT();

  ASTDiagnosticFormatter formatter;
  ASTDiagnosticEmitter emitter(formatter);
  TextDiagnosticConsumer consumer(emitter);

  Compiler compiler;
  compiler.AddDiagnosticConsumer(consumer);

  auto FinishCompile = [&](Status status = Status::Success()) -> int {
    return (status.IsError() ? status.GetFlag() : compiler.GetDiags().Finish());
  };
  // Check for empty args
  if (args.empty()) {
    compiler.GetDiags().PrintD(diag::err_no_compile_args);
    return FinishCompile(Status::Error());
  }
  auto status = compiler.GetInvocation().ParseCommandLine(args);
  if (status.IsError()) {
    return FinishCompile(Status::Error());
  }
  if (!compiler.GetInvocation().HasAction()) {
    // compiler.GetDiags().PrintD(diag::err_no_compile_action);
    FinishCompile(Status::Error());
  }
  compiler.Setup();
  if (compiler.GetExecution().Execute().IsError()) {
    return FinishCompile(Status::Error());
  }

  // CompileStatus status;
  // stone::CompileForSupport(compiler, status);
  // if (!status.ShouldContinue()) {
  //   return FinishCompile(status.GetStatus());
  // }

  // stone::CompileBeforeCodeAnalysis(compiler, status);
  // if (!status.ShouldContinue()) {
  //   return FinishCompile(status.GetStatus());
  // }

  // stone::CompileForCodeAnalysis(compiler, status);
  // if (!status.ShouldContinue()) {
  //   return FinishCompile(status.GetStatus());
  // }

  // stone::CompileAfterCodeAnalysis(compiler, status);
  // if (!status.ShouldContinue()) {
  //   return FinishCompile(status.GetStatus());
  // }

  // stone::CompileForCodeGen(compiler, status);

  // return FinishCompile(status.GetStatus());
}

// void stone::CompileForSupport(Compiler &compiler, CompileStatus &status) {}

// void stone::CompileBeforeCodeAnalysis(Compiler &compiler,
//                                       CompileStatus &status) {}

// void stone::CompileForCodeAnalysis(Compiler &compiler, CompileStatus &status)
// {
//   stone::CompileForSyntaxAnalysis(compiler, status);
//   if (!status.ShouldContinue()) {
//     return;
//   }

//   stone::CompileForSemanticAnalysis(compiler, status);
// }

// void stone::CompileForSyntaxAnalysis(Compiler &compiler,
//                                      CompileStatus &status) {}

// void stone::CompileForSemanticAnalysis(Compiler &compiler,
//                                        CompileStatus &status) {}

// void stone::CompileAfterSemanticAnalysis(Compiler &compiler,
//                                          CompileStatus &status) {}

// void stone::CompileForCodeGen(Compiler &compiler, CompileStatus &status) {

//   stone::CompileForGenIR(compiler, status, codeGenContext);
//   if (!status.ShouldContinue()) {
//     return;
//   }

//   stone::CompileForGenMachine(compiler, status, codeGenContext)
// }

// void stone::CompileForGenIR(Compiler &compiler, CompileStatus &status,
//                             CodeGenContext &codeGenContext) {}

// void stone::CompileForGenMachine(Compiler &compiler, CompileStatus &status,
//                                  CodeGenContext &codeGenContext) {}