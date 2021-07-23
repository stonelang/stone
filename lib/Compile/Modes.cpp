#include "stone/Compile/Modes.h"
#include "stone/Basic/Defer.h"
#include "stone/Basic/Ret.h"
#include "stone/CodeAnalysis/Parse.h"
#include "stone/CodeAnalysis/TypeCheck.h"
#include "stone/CodeGen/Gen.h"
#include "stone/Compile/CompilableItem.h"
#include "stone/Compile/Compiler.h"
#include "stone/Syntax/Module.h"

using namespace stone;

int mode::Parse(CompilableItem &ci, bool check) {

  syn::ParseSyntaxFile(ci.GetSyntaxFile(), ci.GetCompiler().GetSyntax(),
                       ci.GetCompiler().GetPipelineEngine());

  if (ci.GetCompiler().HasError()) {
    return ret::err;
  }
  return ret::ok;
}
int mode::Parse(CompilableItem &ci) { return mode::Parse(ci, false); }
int mode::Check(CompilableItem &ci) { return Parse(ci, true); }

int mode::EmitIR(CompilableItem &ci) {

  /// Should be in EmitIR Scope
  if (!mode::Check(ci)) {
    return ret::err;
  }
  /// Should be in Parse scope
  // if(compiler.GetCompilerContext().GetCompilingScope() !=
  // CompilingScopeType::Parsing) {
  //}
  // If we are here, then parse should have already been called.
  auto llvmModule =
      stone::GenIR(ci.GetCompiler().GetMainModule(), ci.GetCompiler(),
                   ci.GetCompiler().compilerOpts.genOpts, ci.GetOutputFile());

  if (ci.GetCompiler().HasError()) {
    return ret::err;
  }
  ci.GetCompiler().GetCompilerContext().SetLLVMModule(llvmModule);
  return ret::ok;
}

int mode::EmitObject(CompilableItem &ci) {

  if (!ci.GetCompiler().GetMode().CanOutput()) {
    return ret::err;
  }
  ci.CreateOutputFile();

  if (!ci.GetOutputFile()) {
    return ret::err;
  }
  /// Should be in EmitIR Scope
  if (!mode::EmitIR(ci)) {
    return ret::err;
  }

  if (!stone::GenObject(ci.GetCompiler().GetCompilerContext().GetLLVMModule(),
                        ci.GetCompiler().GetCompilerOptions().genOpts,
                        ci.GetCompiler().GetTreeContext(),
                        ci.GetOutputFile())) {
    return ret::err;
  }

  return ret::ok;
}
int mode::EmitAssembly(CompilableItem &ci) { return ret::ok; }
int mode::EmitLibrary(CompilableItem &ci) { return ret::ok; }
int mode::EmitModule(CompilableItem &ci) { return ret::ok; }
int mode::EmitBitCode(CompilableItem &ci) { return ret::ok; }
