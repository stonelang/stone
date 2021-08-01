#include "stone/Compile/CompilerWorkspace.h"
#include "stone/Basic/Defer.h"
#include "stone/Basic/Ret.h"
#include "stone/CodeGen/CodeGenListener.h"
#include "stone/CodeGen/Gen.h"
#include "stone/Compile/Compilable.h"
#include "stone/Compile/Compiler.h"
#include "stone/Compile/CompilerListener.h"
#include "stone/Parse/Parse.h"
#include "stone/Parse/SyntaxListener.h"
#include "stone/Semantics/TypeCheck.h"
#include "stone/Semantics/TypeCheckerListener.h"
#include "stone/Syntax/Module.h"

using namespace stone;

CompilerWorkspace::CompilerWorkspace(Compiler &compiler) : compiler(compiler) {}

void CompilerWorkspace::BuildCompilableFiles() {}

void CompilerWorkspace::CompileFiles() {}

void CompilerWorkspace::CompileFile(CompilableFile &cf) { Parse(cf); }

void CompilerWorkspace::Parse(CompilableFile &cf) {

  auto srcID = compiler.MakeSrcID(cf.GetFile().GetName());
  if (compiler.HasError()) {
    return;
  }
  auto syntaxFile =
      SyntaxFile::Make(SyntaxFileKind::Library, *compiler.GetMainModule(),
                       compiler.GetTreeContext(), srcID);

  if (compiler.HasError()) {
    return;
  }

  syn::ParseSyntaxFile(*syntaxFile, compiler.GetSyntax(),
                       compiler.GetCompilerListener());

  if (compiler.HasError()) {
    return;
  }

  if (compiler.GetCompilerListener()) {
    compiler.GetCompilerListener()->OnParseCompleted(syntaxFile);
  }

  if (compiler.GetMode().Is(ModeType::Parse)) {
    return;
  }
  TypeCheck(syntaxFile);
}

void CompilerWorkspace::TypeCheck(syn::SyntaxFile *sf) {

  sema::TypeCheckSyntaxFile(*sf,
                            compiler.GetCompilerOptions().typeCheckerOptions,
                            compiler.GetCompilerListener());

  if (compiler.GetCompilerListener()) {
    compiler.GetCompilerListener()->OnTypeCheckCompleted(sf);
  }

  if (compiler.GetMode().Is(ModeType::TypeCheck)) {
    return;
  }
  EmitIR(sf);
}

void CompilerWorkspace::EmitIR(syn::SyntaxFile *sf) {

  llvm::Module *ir = nullptr;
  if (compiler.GetCompilerListener()) {
    compiler.GetCompilerListener()->OnEmitIRCompleted(ir);
  }

  if (compiler.GetMode().Is(ModeType::EmitIR)) {
    return;
  }
  switch (compiler.GetMode().GetType()) {
  case ModeType::EmitObject:
    return EmitObject(ir);
  default:
    break; // llvm should not reach
  }
}

void CompilerWorkspace::EmitObject(llvm::Module *ir) {

  if (compiler.GetCompilerListener()) {
    compiler.GetCompilerListener()->OnEmitObjectCompleted();
  }
}
