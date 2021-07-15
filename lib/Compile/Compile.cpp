#include "stone/Compile/Compile.h"
#include "stone/Basic/Defer.h"
#include "stone/Basic/List.h"
#include "stone/Basic/Ret.h"
#include "stone/Compile/Compiler.h"
#include "stone/Compile/Modes.h"
#include "stone/Session/ExecutablePath.h"
#include "stone/Syntax/Module.h"

using namespace stone;
static std::unique_ptr<CompilableItem> BuildCompilable(Compiler &compiler,
                                                       file::File &input) {

  auto fileBuffer = compiler.GetFileMgr().getBufferForFile(input.GetName());
  if (!fileBuffer) {
    compiler.Error(0);
    return nullptr;
  }
  auto srcID = compiler.GetSrcMgr().CreateSrcID(std::move(*fileBuffer));
  compiler.GetSrcMgr().SetMainSrcID(srcID);

  auto sf =
      SyntaxFile::Make(SyntaxFile::Kind::Library, *compiler.GetMainModule(),
                       compiler.GetTreeContext(), srcID);

  assert(sf && "Could not create SyntaxFile");

  std::unique_ptr<CompilableItem> compilable(
      new CompilableItem(CompilableFile(input, false), compiler, *sf));

  // TODO: May want to do tis later
  if (compilable->CanOutput()) {
    compilable->CreateOutputFile();
  }
  return compilable;
}

static int ExecuteMode(CompilableItem &compilable) {
  switch (compilable.GetCompiler().GetMode().GetType()) {
  case ModeType::Parse:
    return mode::Parse(compilable);
  case ModeType::Check:
    return mode::Check(compilable);
  case ModeType::EmitModule:
    return mode::EmitModule(compilable);
  default:
    return mode::EmitObject(compilable);
  }
}
int stone::Compile(Compiler &compiler, file::File &input) {
  auto compilable = BuildCompilable(compiler, input);
  if (!compilable) {
    return ret::err;
  }
  if (!ExecuteMode(*compilable.get())) {
    return ret::err;
  }
  compiler.GetCompilerContext().AddCompilable(std::move(compilable));

  return ret::ok;
}

int Compiler::Run(Compiler &compiler) {

  assert(compiler.GetMode().IsCompilable() && "Invalid compile mode.");
  if (compiler.GetInputFiles().empty()) {
    compiler.Error(0);
    printf("No input files.\n"); // TODO: Use Diagnostics
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
