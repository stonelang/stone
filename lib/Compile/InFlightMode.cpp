#include "stone/Compile/InFlightMode.h"
#include "stone/Basic/Defer.h"
#include "stone/Basic/PipelineEngine.h"
#include "stone/Basic/Ret.h"
#include "stone/CodeGen/CodeGenPipeline.h"
#include "stone/CodeGen/Gen.h"
#include "stone/Compile/Compiler.h"
#include "stone/Parse/Parse.h"
#include "stone/Parse/SyntaxPipeline.h"
#include "stone/Semantics/TypeCheck.h"
#include "stone/Semantics/TypeCheckerPipeline.h"
#include "stone/Syntax/Module.h"

using namespace stone;

InFlightMode::InFlightMode(Compiler &compiler) : compiler(compiler) {}

SyntaxInFlightMode::SyntaxInFlightMode(Compiler &compiler)
    : InFlightMode(compiler) {}

ParseInFlightMode::ParseInFlightMode(Compiler &compiler)
    : SyntaxInFlightMode(compiler) {}

int ParseInFlightMode::Execute() {

  // auto srcID = compiler.MakeSrcID(inFlightInputFile.GetFile().GetName());
  // if (compiler.HasError()) {
  //   return ret::err;
  // }
  // // TODO: Check for errors
  // syntaxFile =
  //     SyntaxFile::Make(SyntaxFileKind::Library, *compiler.GetMainModule(),
  //                      compiler.GetTreeContext(), srcID);

  SyntaxPipeline *pipeline = nullptr;
  if (compiler.GetPipelineEngine()) {
    pipeline = static_cast<SyntaxPipeline *>(
        compiler.GetPipelineEngine()->Get(PipelineType::Syntax));
  }
  // syn::ParseSyntaxFile(syntaxFile, compiler.GetSyntax(), pipeline);

  return ret::ok;
}
void ParseInFlightMode::Finish() {}

TypeCheckInFlightMode::TypeCheckInFlightMode(Compiler &compiler)
    : ParseInFlightMode(compiler) {}

int TypeCheckInFlightMode::Execute() {

  if (!ParseInFlightMode::Execute()) {
    return ret::err;
  }

  TypeCheckerPipeline *pipeline = nullptr;
  if (compiler.GetPipelineEngine()) {
    pipeline = static_cast<TypeCheckerPipeline *>(
        compiler.GetPipelineEngine()->Get(PipelineType::TypeCheck));
  }
  sema::TypeCheckSyntaxFile(
      *syntaxFile, compiler.GetCompilerOptions().typeCheckerOptions, pipeline);
  return ret::ok;
}
void TypeCheckInFlightMode::Finish() {}

EmitIRInFlightMode::EmitIRInFlightMode(Compiler &compiler)
    : TypeCheckInFlightMode(compiler) {}

int EmitIRInFlightMode::Execute() {

  if (!TypeCheckInFlightMode::Execute()) {
    return ret::err;
  }

  if (compiler.GetPipelineEngine()) {
    pipeline = static_cast<CodeGenPipeline *>(
        compiler.GetPipelineEngine()->Get(PipelineType::CodeGen));
  }
  // lvmModule = stone::GenIR(compiler.GetMainModule(), compiler,
  //                          compiler.compilerOpts.genOpts, GetOutputFile());

  // if (GetCompiler().HasError()) {
  //   return ret::err;
  // }
  return ret::ok;
}
void EmitIRInFlightMode::Finish() {}

EmitObjectInFlightMode::EmitObjectInFlightMode(Compiler &compiler)
    : EmitIRInFlightMode(compiler) {}

int EmitObjectInFlightMode::Execute() {

  if (!compiler.GetMode().CanOutput()) {
    return ret::err;
  }

  // GetCompiler().CreateOutputFile();

  // if (!ci.GetOutputFile()) {
  //   return ret::err;
  // }
  /// Should be in EmitIR Scope
  // if (!EmitIRInFlightMode::Execute()){
  //   return ret::err;
  // }
  // if (!stone::GenObject(GetCompiler().GetCompilerContext().GetLLVMModule(),
  //                       GetCompiler().GetCompilerOptions().genOpts,
  //                       GetCompiler().GetTreeContext(),
  //                       GetCompiler().GetOutputFile())) {
  //   return ret::err;
  // }

  return ret::ok;
}
void EmitObjectInFlightMode::Finish() {}

EmitModuleInFlightMode::EmitModuleInFlightMode(Compiler &compiler)
    : EmitIRInFlightMode(compiler) {}

int EmitModuleInFlightMode::Execute() {
  TypeCheckInFlightMode::Execute();
  return ret::ok;
}
void EmitModuleInFlightMode::Finish() {}

EmitBitCodeInFlightMode::EmitBitCodeInFlightMode(Compiler &compiler)
    : EmitIRInFlightMode(compiler) {}

int EmitBitCodeInFlightMode::Execute() {
  EmitIRInFlightMode::Execute();
  return ret::ok;
}
void EmitBitCodeInFlightMode::Finish() {}

// EmitAssemblyInFlightMode::EmitAssemblyInFlightMode() {}
// int EmitAssemblyInFlightMode::Execute() { return ret::ok; }

// EmitLibraryInFlightMode::EmitLibraryInFlightMode() {}
// int EmitLibraryInFlightMode::Execute() { return ret::ok; }
