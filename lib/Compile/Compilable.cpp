
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

  // TODO: Clean this up:
  // Notify external listeners
  if (pipeline && !compiler.HasError()) {
    pipeline->OnSyntaxFileParsed(syntaxFile);
  }
  NotifyListeners();

  if (compiler.HasError()) {
    return ret::err;
  }
  return ret::ok;
}

void SyntaxParsing::NotifyListeners() {
  // Nofify internal listeners
  for (auto listener : listeners) {
    if (listener.GetKind() == PipelineListenerKind::Syntax) {
      auto syntaxParsing = static_cast<SyntaxPipelineListener *>(&listener);
      if (compiler.HasError()) {
        syntaxParsing->OnError();
      } else {
        syntaxParsing->OnSyntaxFileParsed(syntaxFile);
      }
    }
  }
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

void TypeChecking::OnSyntaxFileParsed(syn::SyntaxFile *syntaxFile) {}

void TypeChecking::NotifyListeners() {}

void TypeChecking::Finish() {}

EmittingIR::EmittingIR(Compiler &compiler)
    : OutputCompilable(compiler), typeChecking(compiler) {}

int EmittingIR::DoCompileFile() {

  if (!typeChecking.CompileFile(GetCompilableFile())) {
    return ret::err;
  }

  compiler.GetMainModule()->AddFile(*typeChecking.GetSyntaxFile());
  EmittingIRPipelineListener *pipeline = nullptr;
  if (compiler.GetPipelineEngine()) {
    pipeline = static_cast<EmittingIRPipelineListener *>(
        compiler.GetPipelineEngine()->Get(PipelineListenerKind::CodeGen));
  }
  // lvmModule = stone::GenIR(compiler.GetMainModule(), compiler,
  //                          compiler.compilerOpts.genOpts, GetOutputFile());

  if (compiler.HasError()) {
    return ret::err;
  }
  return ret::ok;
}

void EmittingIR::OnSyntaxFileTypeChecked(syn::SyntaxFile *syntaxFile) {}
void EmittingIR::OnModuleTypeChecked(syn::SyntaxFile *syntaxFile) {}

void EmittingIR::NotifyListeners() {}
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

void EmittingObject::OnIREmitted(llvm::Module *m) {}

void EmittingObject::NotifyListeners() {}
void EmittingObject::Finish() {}

EmittingModule::EmittingModule(Compiler &compiler)
    : OutputCompilable(compiler), emittingIR(compiler) {}

int EmittingModule::DoCompileFile() {

  if (!emittingIR.CompileFile(input)) {
    return ret::err;
  }
  return ret::ok;
}
void EmittingModule::OnIREmitted(llvm::Module *m) {}

void EmittingModule::NotifyListeners() {}

void EmittingModule::Finish() {}

EmittingBitCode::EmittingBitCode(Compiler &compiler)
    : OutputCompilable(compiler), emittingIR(compiler) {}

int EmittingBitCode::DoCompileFile() {

  if (!emittingIR.CompileFile(input)) {
    return ret::err;
  }
  return ret::ok;
}

void EmittingBitCode::OnIREmitted(llvm::Module *m) {}
void EmittingBitCode::NotifyListeners() {}
void EmittingBitCode::Finish() {}

EmittingAssembly::EmittingAssembly(Compiler &compiler)
    : OutputCompilable(compiler), emittingIR(compiler) {}

int EmittingAssembly::DoCompileFile() {

  if (!emittingIR.CompileFile(input)) {
    return ret::err;
  }
  return ret::ok;
}
void EmittingAssembly::OnIREmitted(llvm::Module *m) {}
void EmittingAssembly::NotifyListeners() {}
void EmittingAssembly::Finish() {}

EmittingLibrary::EmittingLibrary(Compiler &compiler)
    : OutputCompilable(compiler), emittingIR(compiler) {}

int EmittingLibrary::DoCompileFile() {

  if (!emittingIR.CompileFile(input)) {
    return ret::err;
  }
  return ret::ok;
}
void EmittingLibrary::OnIREmitted(llvm::Module *m) {}
void EmittingLibrary::NotifyListeners() {}
void EmittingLibrary::Finish() {}
