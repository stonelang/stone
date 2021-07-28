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

int InFlightMode::Execute(const InFlightInputFile &inFlightInputFile) {
  SetInFlightInputFile(inFlightInputFile);
  return ret::ok;
}

SyntaxInFlightMode::SyntaxInFlightMode(Compiler &compiler)
    : InFlightMode(compiler) {}

int SyntaxInFlightMode::Execute(const InFlightInputFile &input) {

  if (!InFlightMode::Execute(input)) {
    return ret::err;
  }
  auto srcID = compiler.MakeSrcID(inFlightInputFile.GetFile().GetName());
  if (compiler.HasError()) {
    return ret::err;
  }
  syntaxFile =
      SyntaxFile::Make(SyntaxFileKind::Library, *compiler.GetMainModule(),
                       compiler.GetTreeContext(), srcID);

  if (compiler.HasError()) {
    return ret::err;
  }

  SyntaxPipeline *pipeline = nullptr;
  if (compiler.GetPipelineEngine()) {
    pipeline = static_cast<SyntaxPipeline *>(
        compiler.GetPipelineEngine()->Get(PipelineType::Syntax));
  }
  syn::ParseSyntaxFile(*syntaxFile, compiler.GetSyntax(), pipeline);

  if (compiler.HasError()) {
    return ret::err;
  }

  return ret::ok;
}
void SyntaxInFlightMode::Finish() {}

TypeCheckInFlightMode::TypeCheckInFlightMode(Compiler &compiler)
    : SyntaxInFlightMode(compiler) {}

int TypeCheckInFlightMode::Execute(const InFlightInputFile &input) {

  if (!SyntaxInFlightMode::Execute(input)) {
    return ret::err;
  }
  TypeCheckerPipeline *pipeline = nullptr;
  if (compiler.GetPipelineEngine()) {
    pipeline = static_cast<TypeCheckerPipeline *>(
        compiler.GetPipelineEngine()->Get(PipelineType::TypeCheck));
  }

  sema::TypeCheckSyntaxFile(
      *syntaxFile, compiler.GetCompilerOptions().typeCheckerOptions, pipeline);

  if (compiler.HasError()) {
    return ret::err;
  }
  return ret::ok;
}
void TypeCheckInFlightMode::Finish() {}

EmitIRInFlightMode::EmitIRInFlightMode(Compiler &compiler)
    : TypeCheckInFlightMode(compiler) {}

int EmitIRInFlightMode::Execute(const InFlightInputFile &input) {

  if (!TypeCheckInFlightMode::Execute(input)) {
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

int EmitObjectInFlightMode::Execute(const InFlightInputFile &input) {

  if (!compiler.GetMode().CanOutput()) {
    return ret::err;
  }

  if (!TypeCheckInFlightMode::Execute(input)) {
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

int EmitModuleInFlightMode::Execute(const InFlightInputFile &input) {
  if (!TypeCheckInFlightMode::Execute(input)) {
    return ret::err;
  }
  return ret::ok;
}
void EmitModuleInFlightMode::Finish() {}

EmitBitCodeInFlightMode::EmitBitCodeInFlightMode(Compiler &compiler)
    : EmitIRInFlightMode(compiler) {}

int EmitBitCodeInFlightMode::Execute(const InFlightInputFile &input) {
  if (!EmitIRInFlightMode::Execute(input)) {
    return ret::err;
  }
  return ret::ok;
}
void EmitBitCodeInFlightMode::Finish() {}

// EmitAssemblyInFlightMode::EmitAssemblyInFlightMode() {}
// int EmitAssemblyInFlightMode::Execute() { return ret::ok; }

// EmitLibraryInFlightMode::EmitLibraryInFlightMode() {}
// int EmitLibraryInFlightMode::Execute() { return ret::ok; }
