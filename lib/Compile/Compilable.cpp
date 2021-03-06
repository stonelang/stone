#include "stone/Compile/Compilable.h"
#include "stone/Basic/Defer.h"
#include "stone/Basic/Ret.h"
#include "stone/CodeGen/CodeGenListener.h"
#include "stone/CodeGen/Gen.h"
#include "stone/Compile/Compiler.h"
#include "stone/Compile/CompilerListener.h"
#include "stone/Parse/Parse.h"
#include "stone/Parse/SyntaxListener.h"
#include "stone/Semantics/TypeCheck.h"
#include "stone/Semantics/TypeCheckerListener.h"
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

  syn::ParseSyntaxFile(*syntaxFile, compiler.GetSyntax(),
                       compiler.GetCompilerListener());

  NotifyListeners();

  if (compiler.HasError()) {
    return ret::err;
  }
  return ret::ok;
}

void SyntaxParsing::NotifyListeners() {
  // Nofify internal listeners
  for (auto listener : listeners) {
    if (listener.GetKind() == PipelineListenerKind::Parsing) {
      auto syntaxParsing = static_cast<SyntaxListener *>(&listener);
      if (compiler.HasError()) {
        syntaxParsing->OnError();
      } else {
        syntaxParsing->OnParseCompleted(syntaxFile);
      }
    }
  }
  // Notify caller
  if (compiler.GetCompilerListener()) {
    compiler.GetCompilerListener()->OnParseCompleted(syntaxFile);
  }
}
void SyntaxParsing::Finish() {}

TypeChecking::TypeChecking(Compiler &compiler) : Compilable(compiler) {}

int TypeChecking::DoCompileFile() { return ret::ok; }

void TypeChecking::OnParseCompleted(syn::SyntaxFile *sf) {

  syntaxFile = sf;
  sema::TypeCheckSyntaxFile(*syntaxFile,
                            compiler.GetCompilerOptions().typeCheckerOptions,
                            compiler.GetCompilerListener());
  NotifyListeners();
}

void TypeChecking::NotifyListeners() {

  // Nofify internal listeners
  for (auto listener : listeners) {
    if (listener.GetKind() == PipelineListenerKind::TypeChecking) {
      auto typeChecking = static_cast<TypeCheckerListener *>(&listener);
      if (compiler.HasError()) {
        // typeChecking->OnError();
      } else {
        typeChecking->OnTypeCheckCompleted(syntaxFile);
      }
    }
  }
  if (compiler.GetCompilerListener()) {
    compiler.GetCompilerListener()->OnTypeCheckCompleted(syntaxFile);
  }
}
void TypeChecking::Finish() {}

EmittingIR::EmittingIR(Compiler &compiler) : Compilable(compiler) {}

int EmittingIR::DoCompileFile() { return ret::ok; }

void EmittingIR::OnTypeCheckCompleted(syn::SyntaxFile *syntaxFile) {

  // lvmModule = stone::GenIR(compiler.GetMainModule(), compiler,
  //                          compiler.compilerOpts.genOpts, GetOutputFile());

  compiler.GetMainModule()->AddFile(*syntaxFile);

  NotifyListeners();
}

void EmittingIR::NotifyListeners() {

  // Nofify internal listeners
  // for (auto listener : listeners) {
  //   if (listener.GetKind() == PipelineListenerKind::TypeChecking) {
  //     auto typeChecking = static_cast<TypeCheckerPipelineListener
  //     *>(&listener); if (compiler.HasError()) {
  //       //typeChecking->OnError();
  //     } else {
  //       typeChecking->OnSyntaxFileTypeChecked(syntaxFile);
  //     }
  //   }
  // }
}
void EmittingIR::Finish() {}

EmittingObject::EmittingObject(Compiler &compiler) : Compilable(compiler) {}

int EmittingObject::DoCompileFile() { return ret::ok; }

void EmittingObject::OnEmitIRCompleted(llvm::Module *m) {

  if (!compiler.GetMode().CanOutput()) {
    return;
  }
  // compiler.CreateOutputFile();
  // if (!ci.GetOutputFile()) {
  //   return ret::err;
  // }

  // stone::GenObject(m, compiler.GetCompilerOptions().genOpts,
  //                  compiler.GetTreeContext(), compiler.GetOutputFile());

  NotifyListeners();
}

void EmittingObject::NotifyListeners() {}
void EmittingObject::Finish() {}

EmittingModule::EmittingModule(Compiler &compiler) : Compilable(compiler) {}
int EmittingModule::DoCompileFile() { return ret::ok; }
void EmittingModule::OnEmitIRCompleted(llvm::Module *m) {}
void EmittingModule::NotifyListeners() {}
void EmittingModule::Finish() {}

EmittingBitCode::EmittingBitCode(Compiler &compiler) : Compilable(compiler) {}
int EmittingBitCode::DoCompileFile() { return ret::ok; }
void EmittingBitCode::OnEmitIRCompleted(llvm::Module *m) {}
void EmittingBitCode::NotifyListeners() {}
void EmittingBitCode::Finish() {}

EmittingAssembly::EmittingAssembly(Compiler &compiler) : Compilable(compiler) {}
int EmittingAssembly::DoCompileFile() { return ret::ok; }
void EmittingAssembly::OnEmitIRCompleted(llvm::Module *m) {}
void EmittingAssembly::NotifyListeners() {}
void EmittingAssembly::Finish() {}

EmittingLibrary::EmittingLibrary(Compiler &compiler) : Compilable(compiler) {}
int EmittingLibrary::DoCompileFile() { return ret::ok; }
void EmittingLibrary::OnEmitIRCompleted(llvm::Module *m) {}
void EmittingLibrary::NotifyListeners() {}
void EmittingLibrary::Finish() {}
