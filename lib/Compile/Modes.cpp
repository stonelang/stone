#include "stone/Compile/Modes.h"
#include "stone/Analyze/Check.h"
#include "stone/Analyze/Parse.h"
#include "stone/Basic/Defer.h"
#include "stone/Basic/Ret.h"
#include "stone/Compile/CompilableItem.h"
#include "stone/Compile/Compiler.h"
#include "stone/Gen/Gen.h"
#include "stone/Syntax/Module.h"

using namespace stone;

int mode::Parse(CompilableItem &compilable, bool check) {

  stone::ParseSyntaxFile(compilable.GetSyntaxFile(),
                         compilable.GetCompiler().GetSyntax(),
                         compilable.GetCompiler().GetPipelineEngine());

  if (compilable.GetCompiler().HasError()) {
    return ret::err;
  }
  return ret::ok;
}
int mode::Parse(CompilableItem &compilable) {
  return mode::Parse(compilable, false);
}
int mode::Check(CompilableItem &compilable) { return Parse(compilable, true); }

int mode::EmitIR(CompilableItem &compilable) {

  /// Should be in EmitIR Scope
  if (!mode::Check(compilable)) {
    return ret::err;
  }
  /// Should be in Parse scope
  // if(compiler.GetCompilerContext().GetCompilingScope() !=
  // CompilingScopeType::Parsing) {
  //}
  // If we are here, then parse should have already been called.
  auto llvmModule = stone::GenIR(compilable.GetCompiler().GetMainModule(),
                                 compilable.GetCompiler(),
                                 compilable.GetCompiler().compilerOpts.genOpts,
                                 compilable.GetOutputFile());

  if (compilable.GetCompiler().HasError()) {
    return ret::err;
  }
  compilable.GetCompiler().GetCompilerContext().SetLLVMModule(llvmModule);
  return ret::ok;
}

int mode::EmitObject(CompilableItem &compilable) {

  if (!compilable.GetCompiler().GetMode().CanOutput()) {
    return ret::err;
  }
  compilable.CreateOutputFile();

  if (!compilable.GetOutputFile()) {
    return ret::err;
  }
  /// Should be in EmitIR Scope
  if (!mode::EmitIR(compilable)) {
    return ret::err;
  }

  if (!stone::GenObject(
          compilable.GetCompiler().GetCompilerContext().GetLLVMModule(),
          compilable.GetCompiler().GetCompilerOptions().genOpts,
          compilable.GetCompiler().GetTreeContext(),
          compilable.GetOutputFile())) {
    return ret::err;
  }

  return ret::ok;
}
int mode::EmitAssembly(CompilableItem &compilable) { return ret::ok; }
int mode::EmitLibrary(CompilableItem &compilable) { return ret::ok; }
int mode::EmitModule(CompilableItem &compilable) { return ret::ok; }
int mode::EmitBitCode(CompilableItem &compilable) { return ret::ok; }
