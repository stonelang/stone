#include "stone/Compile/Compile.h"
#include "stone/Basic/Defer.h"
#include "stone/Basic/LLVMInit.h"
#include "stone/Compile/Compiler.h"
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
  }
  compiler.Setup();
  compiler.GetExecution().ExecuteAction();

  return FinishCompile();
}

Status CompilerExecution::ExecutePrintHelp() { return Status(); }
Status CompilerExecution::ExecutePrintVersion() { return Status(); }
Status CompilerExecution::ExecutePrintFeature() { return Status(); }

Status CompilerExecution::ExecuteParseOnly() {
  assert(currentAction == ActionKind::Parse);
  assert(!GetStages().DidStartParseOnly());
  GetStages().StartParseOnly();

  CompilerExecutionRAII compilerExectutionRAII(*this);

  // for (auto moduleFile :
  //      compiler.GetModuleSystem().GetMainModule()->GetFiles()) {
  //   if (auto *sourceFile = llvm::dyn_cast<SourceFile>(moduleFile)) {
  //     stone::ParseSourceFile(*sourceFile, compiler.GetASTContext(),
  //                            syntaxListener, lexerListener);
  //     sourceFile->stage = SourceFileStage::Parsed;
  //   }
  // }

  GetStages().FinishParseOnly();

  return Status();
}
Status CompilerExecution::ExecuteResolveImports() {
  assert(currentAction == ActionKind::ResolveImports);
  assert(!GetStages().DidStartSyntaxAnalysis());
  GetStages().StartSyntaxAnalysis();

  CompilerExecutionRAII exectutionRAII(*this);

  GetStages().FinishSyntaxAnalysis();

  return Status();
}

Status CompilerExecution::ExecuteDumpSyntax() {
  CompilerExecutionRAII exectutionRAII(*this);
  return Status();
}

Status CompilerExecution::ExecuteTypeCheck() {
  assert(currentAction == ActionKind::TypeCheck);
  assert(GetStages().DidFinishSyntaxAnalysis());
  assert(!GetStages().DidStartTypeChecking());

  GetStages().StartTypeChecking();

  CompilerExecutionRAII exectutionRAII(*this);


  GetStages().FinishTypeChecking();

  return Status();
}


Status CompilerExecution::ExecuteDumpTypeInfo() { return Status(); }
Status CompilerExecution::ExecutePrintSyntax() { return Status(); }
Status CompilerExecution::ExecutePrintIR(CodeGenContext &cgc) { return Status(); }

Status CompilerExecution::ExecuteEmitIRBefore(CodeGenContext& codeGenContext) {
  //assert(!GetStages().StartGeneratingIR());
  return Status();
}

// TODO: Do not need this to be a call back
Status CompilerExecution::GenerateIR(CodeGenContext &cgc) {
  return Status();
}

Status CompilerExecution::ExecuteInitModule() { return Status(); }
Status CompilerExecution::ExecuteEmitModule(CodeGenContext &cgc) { return Status(); }
Status CompilerExecution::ExecuteMergeModules() { return Status(); }

Status CompilerExecution::ExecuteEmitIRAfter(CodeGenContext &cgc) {
  //assert(GetStages().DidFinishGeneratingIR());

  // stone::OptimizeIR();

  return Status();
}
Status CompilerExecution::ExecuteEmitBC(CodeGenContext &cgc) {
  return Status();
}

Status CompilerExecution::ExecuteEmitLibrary(CodeGenContext &cgc) {
  return Status();
}
Status CompilerExecution::ExecuteEmitAssembly(CodeGenContext &cgc) {
  return Status();
}
Status CompilerExecution::ExecuteEmitObject(CodeGenContext& codeGenContext) {
  assert(currentAction == ActionKind::EmitObject);

  assert(GetStages().DidFinishGeneratingIR());

  CompilerExecutionRAII exectutionRAII(*this);

  return Status();
}

CompilerExecutionRAII::CompilerExecutionRAII(CompilerExecution &execution)
    : execution(execution) {}

CompilerExecutionRAII::~CompilerExecutionRAII() {
  /// Print out the time
}
