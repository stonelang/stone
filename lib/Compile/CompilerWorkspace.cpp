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

void CompilerWorkspace::CompileFile(CompilableFile &cf) { ParseFile(cf); }

void CompilerWorkspace::ParseFile(CompilableFile &cf) {

  if (compiler.GetMode().Is(ModeType::Parse)) {
    return;
  }
  TypeCheckFile(nullptr);
}

void CompilerWorkspace::TypeCheckFile(syn::SyntaxFile *sf) {

  if (compiler.GetMode().Is(ModeType::TypeCheck)) {
    return;
  }

  EmitIR(nullptr);
}

void CompilerWorkspace::EmitIR(syn::SyntaxFile *sf) {

  if (compiler.GetMode().Is(ModeType::EmitIR)) {
    return;
  }
  switch (compiler.GetMode().GetType()) {
  case ModeType::EmitObject:
    return EmitObject(nullptr);
  default:
    break; // llvm should not reach
  }
}

void CompilerWorkspace::EmitObject(llvm::Module *m) {}
