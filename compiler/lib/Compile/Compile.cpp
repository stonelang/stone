#include "stone/Compile/Compile.h"
#include "stone/Basic/Defer.h"
#include "stone/Basic/LLVMInit.h"
#include "stone/Basic/MainExecutablePath.h"
#include "stone/Compile/Compiler.h"
#include "stone/Compile/CompilerTask.h"
#include "stone/Compile/Compiling.h"
#include "stone/Diag/CompilerDiagnostic.h"
#include "stone/Diag/TextDiagnosticConsumer.h"
#include "stone/Diag/TextDiagnosticFormatter.h"
#include "stone/Gen/CodeGenContext.h"
#include "stone/Option/Action.h"
#include "stone/Public.h"
#include "stone/Syntax/ASTDiagnosticArgument.h"
#include "stone/Syntax/Module.h"

#include "clang/Basic/TargetInfo.h"

#include "llvm/IR/Module.h"
#include "llvm/Support/Casting.h"

using namespace stone;

int stone::Compile(llvm::ArrayRef<const char *> args, const char *arg0,
                   void *mainAddr, CompilerListener *listener) {

  llvm::PrettyStackTraceString crashInfo("Compile construction...");
  FINISH_LLVM_INIT();

  ASTDiagnosticFormatter formatter;
  ASTDiagnosticEmitter emitter(formatter);
  TextDiagnosticConsumer consumer(emitter);

  CompilerInvocation invocation;
  invocation.AddDiagnosticConsumer(consumer);

  auto FinishCompile = [&](Status status = Status::Success()) -> int {
    return (status.IsError() ? status.GetFlag()
                             : invocation.GetDiags().Finish());
  };
  // Check for empty args
  if (args.empty()) {
    invocation.PrintD(diag::err_no_compile_args);
    return FinishCompile(Status::Error());
  }

  // Now, parse the args
  auto status = invocation.ParseArgs(llvm::ArrayRef<const char *> args);
  if (status.IsError()) {
    return FinishCompile(Status::Error());
  }
  if (listener) {
    listener->CompletedCommandLineParsing(invocation);
  }

  // Set the main execution
  invocation.SetMainExecutable(arg0, mainAddr);

  // Now, use the compiler
  Compiler compiler(invocation);
  status = compiler.Initialize();
  if (status.IsError()) {
    return FinishCompile(Status::Error());
  }
  if (listener) {
    listener->CompletedInitialization(compiler);
  }
  // Otherwise, build out the other tasks
  compiler.BuildTasks();
  if (compiler.HasError()) {
    return FinishCompile(Status::Error());
  }
  if (listener) {
    listener->CompletedBuildingTasks(compiler);
  }
  // Now that we are ready to do real work, call defer Finish
  STONE_DEFER { compiler.Finalize(); };

  // Run compiler tasks
  compiler.RunTasks();
  if (compiler.HasError()) {
    return FinishCompile(Status::Error());
  }
  if (listener) {
    listener->CompletedRunningTasks(compiler);
  }

  return FinishCompile();
}

void Compiler::BuildTasks() { QueueTask(GetAction().GetKind()); }

void Compiler::QueueTask(ActionKind source) {
  switch (source) {
  case ActionKind::PrintHelp: {
    QueueTask(GetTask(ActionKind::PrintHelp));
    break;
  case ActionKind::PrintHelpHidden:
    QueueTask(GetTask(ActionKind::PrintHelpHidden));
    break;
  }
  case ActionKind::PrintVersion: {
    QueueTask(GetTask(ActionKind::PrintVersion));
    break;
  }
  case ActionKind::Parse: {
    QueueTask(GetTask(ActionKind::Parse));
    break;
  }
  case ActionKind::ResolveImports: {
    QueueTask(ActionKind::Parse);
    QueueTask(GetTask(ActionKind::ResolveImportsTask));
    break;
  }
  // case ActionKind::DumpSyntax: {
  //   QueueTask(ActionKind::Parse);
  //   QueueTask(DumpSyntaxTask::Create(*this));
  //   break;
  // }
  case ActionKind::TypeCheck: {
    QueueTask(ActionKind::ResolveImports);
    QueueTask(GetTask(ActionKind::TypeCheckTask));
    break;
  }
    // case ActionKind::PrintSyntax: {
    //   QueueTask(ActionKind::TypeCheck);
    //   QueueTask(PrintSyntaxTask::Create(*this));
    //   break;
    // }
    // case ActionKind::MergeModules: {
    //   QueueTask(ActionKind::TypeCheck);
    //   QueueTask(MergeModulesTask::Create(*this));
    //   break;
    // }

  case ActionKind::EmitIRBefore: {
    QueueTask(ActionKind::TypeCheck));
    QueueTask(GetTask(ActionKind::EmitIRBefore));
    break;
  }
  case ActionKind::EmitIRAfter: {
    QueueTask(ActionKind::TypeCheck))
    QueueTask(GetTask(ActionKind::EmitIRAfterTask));
    break;
  }
  // case ActionKind::EmitBC: {
  //   QueueTask(ActionKind::EmitIRAfter);
  //   QueueTask(EmitBCTask::Create(*this));
  //   break;
  // }
  case ActionKind::EmitObject: {
    QueueTask(ActionKind::TypeCheck))
    QueueTask(GetTask(ActionKind::EmitObject));
    break;
  }
  // case ActionKind::DumpTypeInfo: {
  //   QueueTask(ActionKind::EmitIRAfter);
  //   QueueTask(DumpTypeInfoTask::Create(*this));
  //   break;
  // }
  default: {
    llvm_unreachable("Unknown action -- cannot compile!");
  }
  }
}

void Compiler::RunTasks() {

  // Now, run tasks
  GetQueue().RunTasks();
}

Status PrintHelpTask::Execute(Compiler &compiler, CompilerTask *dep) {

  assert(invocation.GetAction().IsPrintHelp() ||
         invocation.GetAction().IsPrintHelpHidden());

  unsigned IncludedFlagsBitmask = opts::CompilerOption;
  unsigned ExcludedFlagsBitmask =
      invocation.GetAction().IsPrintHelpHidden() ? 0 : llvm::opt::HelpHidden;
  invocation.GetOptions().PrintHelp(
      llvm::outs(), invocation.MainExecutableName.c_str(), "stone-compile",
      IncludedFlagsBitmask, ExcludedFlagsBitmask,
      /*ShowAllAliases*/ false);
  return Status()
}

Status PrintVersionTask::Execute(Compiler &compiler, CompilerTask *dep) {

  return Status();
}

Status AbstractParseTask::Setup(Compiler &compiler) {
  assert([&]() -> bool {
    if (compiler.GetAction().IsParse()) {
      // Parsing gets triggered lazily, but let's make sure we have theright
      // input kind.
      return llvm::all_of(
          compiler.GetCompilerOptions().inputsAndOutputs.GetInputs(),
          [](const CompilerInputFile &cif) {
            const auto fileType = cif.GetType();
            return fileType == file::Type::Stone ||
                   fileType == file::Type::StoneModuleInterface;
          });
    }
    return true;
  }() && "Only supports parsing .stone files");
  return Status();
}

Status ParseTask::Execute(Compiler &compiler, CompilerTask *dep) {}

Status ResolveImportsTask::Execute(Compiler &compiler, CompilerTask *dep) {

  assert(dep);
  assert(dep->IsCompleted());

  return Status();
}

Status TypeCheckTask::Execute(Compiler &compiler, CompilerTask *dep) {
  assert(dep);
  assert(dep->IsCompleted());

  return Status();
}

Status AbstractEmitTask::Setup(Compiler &compiler) {

  /// Perform GenIR
  return Status();
}

Status EmitIRAfterTask::Execute(Compiler &compiler, CompilerTask *dep) {

  return Status();
}

Status EmitIRBeforeTask::Execute(Compiler &compiler, CompilerTask *dep) {

  return Status();
}

Status EmitObjectTask::Execute(Compiler &compiler, CompilerTask *dep) {

  return Status();
}
