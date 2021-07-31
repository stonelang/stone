#include "stone/Compile/Compile.h"
#include "stone/Basic/CompileDiagnostic.h"
#include "stone/Basic/Defer.h"
#include "stone/Basic/List.h"
#include "stone/Basic/Ret.h"
#include "stone/Compile/Compilable.h"
#include "stone/Compile/CompilableFactory.h"
#include "stone/Compile/Compiler.h"
#include "stone/Compile/CompilerListener.h"
#include "stone/Session/ExecutablePath.h"
#include "stone/Syntax/Module.h"

#include <memory>
using namespace stone;

class CompilerWorkspace {
public:
};

static std::unique_ptr<Compilable> MakeCompilable(Compiler &compiler) {
  switch (compiler.GetMode().GetType()) {
  case ModeType::Parse:
    return CompilableFactory::MakeSyntaxParsing(compiler);
  case ModeType::TypeCheck:
    return CompilableFactory::MakeTypeChecking(compiler);
  case ModeType::EmitIR:
    return CompilableFactory::MakeEmittingIR(compiler);
  case ModeType::EmitObject:
    return CompilableFactory::MakeEmittingObject(compiler);
  case ModeType::EmitModule:
    return CompilableFactory::MakeEmittingModule(compiler);
  default:
    llvm_unreachable("Invalid compiler mode!");
  }
}
int Compiler::Run(Compiler &compiler) {

  assert(compiler.GetMode().IsCompilable() && "Invalid compile mode.");

  if (compiler.GetInputFiles().empty()) {
    compiler.GetDiagEngine().Diagnose(SrcLoc(), diag::err_no_input_files);
    return ret::err;
  }
  if (compiler.GetCompilerListener()) {
    compiler.GetCompilerListener()->OnPreCompile(compiler);
  }
  // TODO: Build out the InFlightInputFiles
  // workspace.BuildInFlightInputFiles();
  // for (auto &input : workspace.GetInFlightInputFiles()) {
  // }
  auto compilable = MakeCompilable(compiler);
  for (auto &input : compiler.GetInputFiles()) {
    compilable->CompileFile(CompilableFile(&input, false));
    compilable->NotifyListeners();
    if (compiler.HasError()) {
      return ret::err;
    }
  }
  compilable->Finish();
  if (compiler.GetCompilerListener()) {
    compiler.GetCompilerListener()->OnPostCompile(compiler);
  }
  return ret::ok;
}
int stone::Compile(llvm::ArrayRef<const char *> args, const char *arg0,
                   void *mainAddr, CompilerListener *cl) {

  auto executablePath = stone::GetExecutablePath(arg0);
  Compiler compiler(cl);
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
