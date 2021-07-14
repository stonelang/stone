#include "stone/Compile/CompileInputFile.h"
#include "stone/Analyze/Check.h"
#include "stone/Analyze/Parse.h"
#include "stone/Basic/Defer.h"
#include "stone/Basic/List.h"
#include "stone/Basic/Ret.h"
#include "stone/Compile/CompilableItem.h"
#include "stone/Compile/Compiler.h"
#include "stone/Gen/Gen.h"
#include "stone/Syntax/Module.h"

using namespace stone;




int stone::Parse(CompilableItem &compilable, bool check) {

  stone::ParseSyntaxFile(compilable.GetSyntaxFile(),
                         compilable.GetCompiler().GetSyntax(),
                         compilable.GetCompiler().GetPipelineEngine());

  if (compilable.GetCompiler().HasError()) {
    return ret::err;
  }
  return ret::ok;
}
int stone::Parse(CompilableItem &compilable) {
  return stone::Parse(compilable, false);
}
int stone::Check(CompilableItem &compilable) { return Parse(compilable, true); }

int stone::EmitIR(CompilableItem &compilable) {

  /// Should be in EmitIR Scope
  if (!stone::Check(compilable)) {
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

int stone::EmitObject(CompilableItem &compilable) {

  if (!compilable.GetCompiler().GetMode().CanOutput()) {
    return ret::err;
  }
  compilable.CreateOutputFile();

  if (!compilable.GetOutputFile()) {
    return ret::err;
  }
  /// Should be in EmitIR Scope
  if (!stone::EmitIR(compilable)) {
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
int stone::EmitAssembly(CompilableItem &compilable) { return ret::ok; }
int stone::EmitLibrary(CompilableItem &compilable) { return ret::ok; }
int stone::EmitModule(CompilableItem &compilable) { return ret::ok; }
int stone::EmitBitCode(CompilableItem &compilable) { return ret::ok; }

static int ExecuteCompilable(CompilableItem &compilable) {
  switch (compilable.GetCompiler().GetMode().GetType()) {
  case ModeType::Parse:
    return stone::Parse(compilable);
  case ModeType::Check:
    return stone::Check(compilable);
  case ModeType::EmitModule:
    return stone::EmitModule(compilable);
  default:
    return stone::EmitObject(compilable);
  }
}
static syn::SyntaxFile *BuildSyntaxFileForMainModule(SyntaxFile::Kind kind,
                                                     Compiler &compiler,
                                                     SrcID srcID,
                                                     bool isPrimary) {

  auto *syntaxFile = new (compiler.GetTreeContext())
      SyntaxFile(kind, *compiler.GetMainModule(), srcID, isPrimary);

  return syntaxFile;
}

static std::unique_ptr<CompilableItem> BuildCompilable(Compiler &compiler,
                                                       file::File &input) {

  auto fileBuffer = compiler.GetFileMgr().getBufferForFile(input.GetName());
  if (!fileBuffer) {
    compiler.Error(0);
    return nullptr;
  }
  auto srcID = compiler.GetSrcMgr().CreateSrcID(std::move(*fileBuffer));
  compiler.GetSrcMgr().SetMainSrcID(srcID);

  auto sf = BuildSyntaxFileForMainModule(SyntaxFile::Kind::Library, compiler,
                                         srcID, false);

  assert(sf && "Could not create SyntaxFile");

  std::unique_ptr<CompilableItem> compilable(
      new CompilableItem(CompilableFile(input, false), compiler, *sf));

  // TODO: May want to do tis later
  if (compilable->CanOutput()) {
    compilable->CreateOutputFile();
  }
  return compilable;
}

std::unique_ptr<CompilableItem> stone::CompileInputFile(Compiler &compiler,
                                                        file::File &input) {
  auto compilable = BuildCompilable(compiler, input);
  if (!compilable) {
    return nullptr;
  }
  if (!ExecuteCompilable(*compilable.get())) {
    return nullptr;
  }
  return compilable;
}
