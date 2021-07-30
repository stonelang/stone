
#include "stone/Compile/Compilable.h"
#include "stone/Basic/Defer.h"
#include "stone/Basic/Ret.h"
#include "stone/CodeGen/CodeGenPipelineListener.h"
#include "stone/CodeGen/Gen.h"
#include "stone/Compile/Compiler.h"
#include "stone/Parse/Parse.h"
#include "stone/Parse/SyntaxPipelineListener.h"
#include "stone/Semantics/TypeCheck.h"
#include "stone/Semantics/TypeCheckerPipelineListener.h"
#include "stone/Syntax/Module.h"

using namespace stone;

std::unique_ptr<SyntaxParsing>
CompilableFactory::MakeSyntaxParsing(Compiler &compiler) {

  return std::make_unique<SyntaxParsing>(compiler);
}
std::unique_ptr<TypeChecking>
CompilableFactory::MakeTypeChecking(Compiler &compiler) {
  return std::make_unique<TypeChecking>(compiler);
}

std::unique_ptr<EmittingIR>
CompilableFactory::MakeEmittingIR(Compiler &compiler) {
  return std::make_unique<EmittingIR>(compiler);
}

std::unique_ptr<EmittingObject>
CompilableFactory::MakeEmittingObject(Compiler &compiler) {
  return std::make_unique<EmittingObject>(compiler);
}

std::unique_ptr<EmittingBitCode>
CompilableFactory::MakeEmittingBitCode(Compiler &compiler) {
  return std::make_unique<EmittingBitCode>(compiler);
}

std::unique_ptr<EmittingModule>
CompilableFactory::MakeEmittingModule(Compiler &compiler) {
  return std::make_unique<EmittingModule>(compiler);
}

std::unique_ptr<EmittingAssembly>
CompilableFactory::MakeEmittingAssembly(Compiler &compiler) {
  return std::make_unique<EmittingAssembly>(compiler);
}

Compilable::Compilable(Compiler &compiler) : compiler(compiler) {}

int Compilable::CompileFile(const CompilableFile &input) {
  SetCompilableFile(input);
  return DoCompileFile();
}

void Compilable::Finish() {}

SyntaxParsing::SyntaxParsing(Compiler &compiler) : Compilable(compiler) {}

int SyntaxParsing::DoCompileFile() {

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

  SyntaxPipelineListener *pipeline = nullptr;
  if (compiler.GetPipelineEngine()) {
    pipeline = static_cast<SyntaxPipelineListener *>(
        compiler.GetPipelineEngine()->Get(PipelineListenerKind::Syntax));
  }
  syn::ParseSyntaxFile(*syntaxFile, compiler.GetSyntax(), pipeline);

  if (compiler.HasError()) {
    return ret::err;
  }
  if (pipeline) {
    pipeline->OnSyntaxFile(syntaxFile);
  }
  return ret::ok;
}
void SyntaxParsing::Finish() {}

TypeChecking::TypeChecking(Compiler &compiler)
    : Compilable(compiler), syntaxParsing(compiler) {}

int TypeChecking::DoCompileFile() {

  if (!syntaxParsing.CompileFile(GetCompilableFile())) {
    return ret::err;
  }
  TypeCheckerPipelineListener *pipeline = nullptr;
  if (compiler.GetPipelineEngine()) {
    pipeline = static_cast<TypeCheckerPipelineListener *>(
        compiler.GetPipelineEngine()->Get(PipelineListenerKind::TypeCheck));
  }
  sema::TypeCheckSyntaxFile(*syntaxParsing.GetSyntaxFile(),
                            compiler.GetCompilerOptions().typeCheckerOptions,
                            pipeline);

  if (compiler.HasError()) {
    return ret::err;
  }
  if (pipeline) {
    pipeline->OnSyntaxFileTypeChecked(syntaxParsing.GetSyntaxFile());
  }
  return ret::ok;
}
void TypeChecking::Finish() {}

EmittingIR::EmittingIR(Compiler &compiler)
    : OutputCompilable(compiler), typeChecking(compiler) {}

int EmittingIR::DoCompileFile() {

  if (!typeChecking.CompileFile(GetCompilableFile())) {
    return ret::err;
  }

  compiler.GetMainModule()->AddFile(*typeChecking.GetSyntaxFile());
  CodeGenPipelineListener *pipeline = nullptr;
  if (compiler.GetPipelineEngine()) {
    pipeline = static_cast<CodeGenPipelineListener *>(
        compiler.GetPipelineEngine()->Get(PipelineListenerKind::CodeGen));
  }
  // lvmModule = stone::GenIR(compiler.GetMainModule(), compiler,
  //                          compiler.compilerOpts.genOpts, GetOutputFile());

  if (compiler.HasError()) {
    return ret::err;
  }
  return ret::ok;
}
void EmittingIR::Finish() {}

EmittingObject::EmittingObject(Compiler &compiler)
    : OutputCompilable(compiler), emittingIR(compiler) {}

int EmittingObject::DoCompileFile() {

  if (!compiler.GetMode().CanOutput()) {
    return ret::err;
  }
  if (!emittingIR.CompileFile(input)) {
    return ret::err;
  }

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
    : OutputCompilable(compiler), emittingIR(compiler) {}

int EmittingModule::DoCompileFile() {

  if (!emittingIR.CompileFile(input)) {
    return ret::err;
  }
  return ret::ok;
}
void EmittingModule::Finish() {}

EmittingBitCode::EmittingBitCode(Compiler &compiler)
    : OutputCompilable(compiler), emittingIR(compiler) {}

int EmittingBitCode::DoCompileFile() {

  if (!emittingIR.CompileFile(input)) {
    return ret::err;
  }
  return ret::ok;
}
void EmittingBitCode::Finish() {}

EmittingAssembly::EmittingAssembly(Compiler &compiler)
    : OutputCompilable(compiler), emittingIR(compiler) {}

int EmittingAssembly::DoCompileFile() {

  if (!emittingIR.CompileFile(input)) {
    return ret::err;
  }
  return ret::ok;
}
void EmittingAssembly::Finish() {}

EmittingLibrary::EmittingLibrary(Compiler &compiler)
    : OutputCompilable(compiler), emittingIR(compiler) {}

int EmittingLibrary::DoCompileFile() {

  if (!emittingIR.CompileFile(input)) {
    return ret::err;
  }
  return ret::ok;
}
void EmittingLibrary::Finish() {}
