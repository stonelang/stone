#include "stone/Compile/Compile.h"
#include "stone/Basic/Defer.h"
#include "stone/Basic/List.h"
#include "stone/Basic/Ret.h"
#include "stone/Compile/CompileInputFile.h"
#include "stone/Compile/Compiler.h"
#include "stone/Session/ExecutablePath.h"
#include "stone/Syntax/Module.h"

using namespace stone;

int Compiler::Run(Compiler &compiler) {

  assert(compiler.GetMode().IsCompilable() && "Invalid compile mode.");
  if (compiler.GetInputFiles().empty()) {
    compiler.Error(0);
    printf("No input files.\n"); // TODO: Use Diagnostics
    return ret::err;
  }
  SafeList<CompilableItem> compilables;

  for (auto &input : compiler.GetInputFiles()) {
    auto compilable = stone::CompileInputFile(compiler, input);
    compilables.Add(std::move(compilable));
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
