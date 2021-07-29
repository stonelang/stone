#include "stone/Compile/Compilable.h"
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

Compilable::Compilable(Compiler &compiler) : compiler(compiler) {}

int Compilable::CompileFile(const mode::CompilableFile &input) {
  SetCompilableFile(input);
  return CompileFile();
}

void Compilable::Finish() {}

SyntaxParsing::SyntaxParsing(Compiler &compiler) : Compilable(compiler) {}

int SyntaxParsing::CompileFile() {

  auto srcID = compiler.MakeSrcID(input.GetFile().GetName());
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
void SyntaxParsing::Finish() {}

TypeChecking::TypeChecking(Compiler &compiler)
    : Compilable(compiler), syntaxParsing(compiler) {}

int TypeChecking::CompileFile() {

  // if (!syntaxParsing.CompileFile(input)) {
  //   return ret::err;
  // }
  TypeCheckerPipeline *pipeline = nullptr;
  if (compiler.GetPipelineEngine()) {
    pipeline = static_cast<TypeCheckerPipeline *>(
        compiler.GetPipelineEngine()->Get(PipelineType::TypeCheck));
  }
  // sema::TypeCheckSyntaxFile(
  //     *syntaxFile, compiler.GetCompilerOptions().typeCheckerOptions,
  //     pipeline);

  if (compiler.HasError()) {
    return ret::err;
  }
  return ret::ok;
}
void TypeChecking::Finish() {}

EmittingIR::EmittingIR(Compiler &compiler)
    : OutputableCompilable(compiler), typeChecking(compiler) {}

int EmittingIR::CompileFile() {

  // if (!typeChecking.CompileFile(input)) {
  //   return ret::err;
  // }

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
void EmittingIR::Finish() {}

EmittingObject::EmittingObject(Compiler &compiler)
    : OutputableCompilable(compiler), emittingIR(compiler) {}

int EmittingObject::CompileFile() {

  if (!compiler.GetMode().CanOutput()) {
    return ret::err;
  }
  // if (!emittingIR.CompileFile(input)) {
  //   return ret::err;
  // }

  // GetCompiler().CreateOutputFile();

  // if (!ci.GetOutputFile()) {
  //   return ret::err;
  // }
  /// Should be in EmitIR Scope
  // if (!EmitIRModable::Execute()){
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
void EmittingObject::Finish() {}

EmittingModule::EmittingModule(Compiler &compiler)
    : OutputableCompilable(compiler), emittingIR(compiler) {}

int EmittingModule::CompileFile() {

  // if (!typeChecking.CompileFile(compilableFile)) {
  //   return ret::err;
  // }
  return ret::ok;
}
void EmittingModule::Finish() {}

EmittingBitCode::EmittingBitCode(Compiler &compiler)
    : OutputableCompilable(compiler), emittingIR(compiler) {}

int EmittingBitCode::CompileFile() {

  // if (!emittingIR.CompileFile(input)) {
  //   return ret::err;
  // }
  return ret::ok;
}
void EmittingBitCode::Finish() {}

EmittingAssembly::EmittingAssembly(Compiler &compiler)
    : OutputableCompilable(compiler), emittingIR(compiler) {}

int EmittingAssembly::CompileFile() {

  // if (!emittingIR::CompileFile(input)) {
  //   return ret::err;
  // }
  return ret::ok;
}
void EmittingAssembly::Finish() {}

EmittingLibrary::EmittingLibrary(Compiler &compiler)
    : OutputableCompilable(compiler), emittingIR(compiler) {}

int EmittingLibrary::CompileFile() {

  // if (!emittingIR::CompileFile(input)) {
  //   return ret::err;
  // }
  return ret::ok;
}
void EmittingLibrary::Finish() {}
