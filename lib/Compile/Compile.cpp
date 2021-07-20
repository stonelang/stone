#include "stone/Compile/Compile.h"
#include "stone/Compile/Compiler.h"
#include "stone/Compile/Modes.h"
#include "stone/Core/CompileDiagnostic.h"
#include "stone/Core/Defer.h"
#include "stone/Core/List.h"
#include "stone/Core/Ret.h"
#include "stone/Session/ExecutablePath.h"
#include "stone/Syntax/Module.h"

using namespace stone;

static std::unique_ptr<CompilableItem> BuildCompilable(Compiler &compiler,
                                                       file::File &input) {

  auto fileBuffer = compiler.GetFileMgr().getBufferForFile(input.GetName());
  if (!fileBuffer) {
    compiler.Diagnose(SrcLoc(), diag::err_unable_to_open_filebuffer,
                      diag::LLVMStrArgument(input.GetName()));
    return nullptr;
  }
  auto srcID = compiler.GetSrcMgr().CreateSrcID(std::move(*fileBuffer));
  compiler.GetSrcMgr().SetMainSrcID(srcID);

  auto sf =
      SyntaxFile::Make(SyntaxFile::Kind::Library, *compiler.GetMainModule(),
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
  case ModeType::Check:
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

int Compiler::Run(Compiler &compiler) {

  assert(compiler.GetMode().IsCompilable() && "Invalid compile mode.");
  if (compiler.GetInputFiles().empty()) {
    compiler.GetDiagEngine().Diagnose(SrcLoc(), diag::err_no_input_files);
    return ret::err;
  }
  for (auto &input : compiler.GetInputFiles()) {
    if (!stone::Compile(compiler, input)) {
      ret::err;
    }
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
