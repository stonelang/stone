
#include "stone/Compile/Compile.h"
#include "stone/Basic/CompileDiagnostic.h"
#include "stone/Basic/Defer.h"
#include "stone/Basic/List.h"
#include "stone/Basic/Ret.h"
#include "stone/Compile/Compiler.h"
#include "stone/Compile/InFlightMode.h"
#include "stone/Compile/Modes.h"
#include "stone/Session/ExecutablePath.h"
#include "stone/Syntax/Module.h"

using namespace stone;

std::unique_ptr<InFlightMode> stone::GetInFlightMode(Compiler &compiler) {
  switch (compiler.GetMode().GetType()) {
  case ModeType::Parse:
    return std::make_unique<SyntaxInFlightMode>(compiler);
  case ModeType::TypeCheck:
    return std::make_unique<TypeCheckInFlightMode>(compiler);
  case ModeType::EmitIR:
    return std::make_unique<EmitIRInFlightMode>(compiler);
  case ModeType::EmitObject:
    return std::make_unique<EmitObjectInFlightMode>(compiler);
  default:
    llvm_unreachable("Invalid compiler mode!");
  }
}

static std::unique_ptr<CompilableItem> BuildCompilable(Compiler &compiler,
                                                       file::File &input) {

  auto srcID = compiler.MakeSrcID(input.GetName());
  if (compiler.HasError()) {
    return nullptr;
  }
  // TODO: Check for errors
  auto sf = SyntaxFile::Make(SyntaxFileKind::Library, *compiler.GetMainModule(),
                             compiler.GetTreeContext(), srcID);

  assert(sf && "Could not create SyntaxFile");

  std::unique_ptr<CompilableItem> ci(
      new CompilableItem(CompilableFile(input, false), compiler, *sf));

  // TODO: May want to do tis later
  if (ci->CanOutput()) {
    ci->CreateOutputFile();
  }
  return ci;
}

int mode::Execute(CompilableItem &ci) {
  switch (ci.GetCompiler().GetMode().GetType()) {
  case ModeType::Parse:
    return mode::Parse(ci);
  case ModeType::TypeCheck:
    return mode::Check(ci);
  case ModeType::EmitModule:
    return mode::EmitModule(ci);
  default:
    return mode::EmitObject(ci);
  }
}
int stone::Compile(Compiler &compiler, file::File &input) {
  auto ci = BuildCompilable(compiler, input);
  if (!ci) {
    return ret::err;
  }
  if (!mode::Execute(*ci.get())) {
    return ret::err;
  }
  compiler.GetCompilerContext().AddCompilable(std::move(ci));

  return ret::ok;
}

int stone::Compile(InFlightMode &inFlight) {
  // assert(inFlight.GetInFlightFile() && "No InFlightFile");
}

int Compiler::Run(Compiler &compiler) {

  assert(compiler.GetMode().IsCompilable() && "Invalid compile mode.");

  if (compiler.GetInputFiles().empty()) {
    compiler.GetDiagEngine().Diagnose(SrcLoc(), diag::err_no_input_files);
    return ret::err;
  }
  auto inFlightMode = GetInFlightMode(compiler);
  // TODO: Build out the InFlightInputFiles

  // workspace.BuildInFlightInputFiles();

  // for (auto &input : workspace.GetInFlightInputFiles()) {
  // }

  for (auto &input : compiler.GetInputFiles()) {
    // auto inFlightInputFile = std::make_unique<InFlightInputFile>()
    // inFlightMode->SetInFlightInputFile(std::move(std::unique_ptr<InFlightFile(input,
    // false));
    // inFlightMode->Execute();

    // if (!stone::Compile(inFlightMode, input)) {
    //   ret::err;
    // }
  }
  return ret::ok;
}
int stone::Compile(llvm::ArrayRef<const char *> args, const char *arg0,
                   void *mainAddr, PipelineEngine *pe) {

  auto executablePath = stone::GetExecutablePath(arg0);
  Compiler compiler(pe);
  STONE_DEFER { compiler.Finish(); };

  compiler.Init();
  if (compiler.Build(args)) {
    if (compiler.HasError()) {
      return ret::err;
    }
    assert(compiler.GetMode().IsCompileOnly() && "Not a compile mode");
  }
  return compiler.Run();
}
