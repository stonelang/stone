#include "stone/Compile/Compile.h"
#include "stone/Basic/Defer.h"
#include "stone/Basic/LLVMContext.h"
#include "stone/Basic/LLVMInit.h"
#include "stone/Basic/MainExecutablePath.h"
#include "stone/Compile/CompilerInstance.h"
#include "stone/Compile/CompilerInvocation.h"
#include "stone/CodeCompletionListener.h"
#include "stone/Compile/TargetMachine.h"
#include "stone/Diag/CompilerDiagnostic.h"
#include "stone/Diag/TextDiagnosticFormatter.h"
#include "stone/Diag/TextDiagnosticListener.h"
#include "stone/Gen/CodeGenContext.h"
#include "stone/Gen/Gen.h"
#include "stone/Parse/Parse.h"
#include "stone/Sem/TypeCheck.h"
#include "stone/Sem/UsingResolution.h"
#include "stone/Session/ModeKind.h"
#include "stone/Syntax/Module.h"
#include "stone/Syntax/SyntaxDiagnosticArgument.h"

#include "llvm/IR/Module.h"

using namespace stone;
using namespace stone::syn;
using namespace stone::sem;

/// A PrettyStackTraceEntry to print compiling information
class CompilerPrettyStackTrace : public llvm::PrettyStackTraceEntry {
  const CompilerInvocation &invocation;

public:
  CompilerPrettyStackTrace(const CompilerInvocation &invocation)
      : invocation(invocation) {}

  void print(llvm::raw_ostream &os) const override {

    //   auto effective =
    //   invocation.GetCompilerOptions().effectiveCompilerVersion; if (effective
    //   != version::Version::GetCurrentCompilerVersion()) {
    //     os << "Compiling with effective version " << effective;
    //   } else {
    //     os << "Compiling with the current invocationuage version";
    //   }
    //   if (Invocation.GetCompilerOptions().allowModuleWithCompilerErrors) {
    //     os << " while allowing modules with compiler errors";
    //   }
    //   os << "\n";
  }
};

int stone::Compile(llvm::ArrayRef<const char *> args, const char *arg0,
                   void *mainAddr, CompilerListener *listener) {
  llvm::PrettyStackTraceString crashInfo("Compile construction...");
  FINISH_LLVM_INIT();

  auto Finish = [&](int status = 0) -> int {
    int err = 1;
    return status ? status : err;
  };

  auto programPath = llvm::sys::fs::getMainExecutable(arg0, mainAddr);
  auto programName = file::GetStem(programPath);

  CompilerInvocation invocation(programName, programPath, listener);
  STONE_DEFER { invocation.Finish(); };

  if (args.empty()) {
    invocation.GetLangContext().GetDiagUnit().PrintD(SrcLoc(),
                                                     diag::err_no_input_files);
    return Finish(1);
  }
  // Setup the custom formatting to be able to handle syntax diagnostics
  auto diagFormatter = std::make_unique<SyntaxDiagnosticFormatter>();
  auto diagEmitter =
      std::make_unique<TextDiagnosticEmitter>(std::move(diagFormatter));

  TextDiagnosticListener diagListener(std::move(diagEmitter));
  invocation.GetLangContext().GetDiagUnit().GetDiagEngine().AddListener(
      diagListener);

  // Parse arguments.
  llvm::SmallVector<std::unique_ptr<llvm::MemoryBuffer>, 4>
      configurationFileBuffers;

  auto ial = invocation.ParseArgs(args);
  if (!ial) {
    return Finish(1);
  }
  if (invocation.HasError()) {
    return Finish(1);
  }
  if (invocation.ComputeOptions(*ial).Has()) {
    return Finish(1);
  }
  if (invocation.GetCompilerOptions().GetMode().IsAlien()) {
    invocation.GetLangContext().GetDiagUnit().PrintD(SrcLoc(),
                                                     diag::err_alien_mode);
    Finish(1);
  }
  if (invocation.GetCompilerOptions().GetMode().IsPrintHelp()) {
    // TODO: invocation.PrintHelp(invocation.GetOpts());
    return Finish();
  }
  if (invocation.GetCompilerOptions().GetMode().IsPrintVersion()) {
    invocation.PrintVersion();
    return Finish();
  }
  if (!invocation.GetCompilerOptions().GetMode().CanCompile()) {
    /// invocation.PrintD()
    return Finish(1);
  }
  if (invocation.GetListener()) {
    invocation.GetListener()->OnCompileConfigured(invocation);
  }
  if (invocation.CreateSourceBuffers().Has()) {
    return Finish(1);
  }

  CompilerInstance compiler(invocation);
  compiler.Compile();

  if (invocation.HasError()) {
    return Finish(1);
  }
  return Finish();
}

static void DumpIR(CompilerInstance &compiler, CodeGenContext &cgc,
                   IRCodeGenResult &result) {}

static void PrintIR(CompilerInstance &compiler, CodeGenContext &cgc,
                    IRCodeGenResult &result) {}

using CompileWithGenIRCallback = llvm::function_ref<void(
    CompilerInstance &compiler, CodeGenContext &cgc, IRCodeGenResult &result)>;

static void CompileWithGenIR(CompilerInstance &compiler,
                             stone::ModuleSyntaxFileUnion msf,
                             CodeGenContext &cgc,
                             CompileWithGenIRCallback client) {

  switch (compiler.GetModuleOutputMode()) {
  case ModuleOutputMode::Single: {
    if (auto sf = msf.dyn_cast<SyntaxFile *>()) {
      auto result = stone::GenIR(
          cgc, *sf, compiler.GetInvocation().GetLangContext(), nullptr);
      client(compiler, cgc, *result);
    }
    break;
  }
  case ModuleOutputMode::Whole: {
    if (auto mod = msf.get<syn::Module *>()) {
      auto result = stone::GenIR(
          cgc, *mod, compiler.GetInvocation().GetLangContext(), nullptr);
      client(compiler, cgc, *result);
    }
    break;
  }
  default:
    stone::Panic("Unable to GenIR -- invalid IR ouput");
  }
}

static void GenModule(CompilerInstance &compiler, CodeGenContext &cgc,
                      IRCodeGenResult &result) {}

static void CompileWithGenNative(CompilerInstance &compiler,
                                 CodeGenContext &cgc, IRCodeGenResult &result) {

  auto targetMachine = stone::CreateTargetMachine(
      compiler.GetInvocation().GetLangContext().GetDiagUnit().GetDiagEngine(),
      compiler.GetInvocation().GetCodeGenOptions(),
      compiler.GetInvocation().GetTargetOptions(),
      compiler.GetInvocation().GetLangContext().GetLangOptions(),
      compiler.GetSyntax().GetSyntaxContext(), *result.GetLLVMModule());

  cgc.TakeTargetMachine(std::move(targetMachine));

  auto ComputeNativeModeKind = [&](CompilerInstance &compiler) -> void {
    switch (compiler.GetInvocation().GetCompilerOptions().GetMode().GetKind()) {
    case ModeKind::None:
    case ModeKind::EmitObject:
      compiler.GetInvocation().GetCodeGenOptions().nativeModeKind =
          NativeModeKind::EmitObject;
      break;
    case ModeKind::EmitBC:
      compiler.GetInvocation().GetCodeGenOptions().nativeModeKind =
          NativeModeKind::EmitBC;
      break;
    case ModeKind::EmitAssembly:
      compiler.GetInvocation().GetCodeGenOptions().nativeModeKind =
          NativeModeKind::EmitAssembly;
      break;
    default:
      stone::Panic("Unknown Native mode kind");
    }
  };
  ComputeNativeModeKind(compiler);
  auto err = stone::GenNative(cgc, compiler.GetSyntax().GetSyntaxContext(),
                              result, nullptr);
}

static void CompileWithCodeGen(CompilerInstance &compiler) {

  assert(compiler.GetInvocation().GetCompilerOptions().GetMode().CanCodeGen());

  // We are performing some low level code generation
  CodeGenContext cgc(stone::GetLLVMContext(),
                     compiler.GetInvocation().GetCodeGenOptions(),
                     compiler.GetInvocation().GetLangContext());

  auto *mainModule = compiler.GetModuleSystem().GetMainModule();
  // switch
  // (invocation.GetCompilerOptions().moduleOutputMode)

  switch (compiler.GetInvocation().GetCompilerOptions().GetMode().GetKind()) {
  case ModeKind::EmitModule:
    return CompileWithGenIR(compiler, mainModule, cgc,
                            [&](CompilerInstance &compiler, CodeGenContext &cgc,
                                IRCodeGenResult &result) {
                              return GenModule(compiler, cgc, result);
                            });
  case ModeKind::EmitIR:
    return CompileWithGenIR(
        compiler, mainModule, cgc,
        [&](CompilerInstance &compiler, CodeGenContext &cgc,
            IRCodeGenResult &result) { return DumpIR(compiler, cgc, result); });
  case ModeKind::PrintIR:
    return CompileWithGenIR(compiler, mainModule, cgc,
                            [&](CompilerInstance &compiler, CodeGenContext &cgc,
                                IRCodeGenResult &result) {
                              return PrintIR(compiler, cgc, result);
                            });
  default:
    return CompileWithGenIR(compiler, mainModule, cgc,
                            [&](CompilerInstance &compiler, CodeGenContext &cgc,
                                IRCodeGenResult &result) {
                              return CompileWithGenNative(compiler, cgc,
                                                          result);
                            });
  }
}

static void DumpSyntax(syn::SyntaxFile &sf) {}

static void PrintSyntax(CompilerInstance &compiler) {}

void CompilerInstance::ForEachSyntaxFile(EachSyntaxFileCallback client) {

  switch (invocation.GetTypeCheckMode()) {
  case TypeCheckMode::WholeModule: {
    for (auto moduleFile : GetModuleSystem().GetMainModule()->GetFiles()) {
      auto *syntaxFile = dyn_cast<SyntaxFile>(moduleFile);
      if (syntaxFile) {
        client(*syntaxFile, invocation.GetTypeCheckerOptions(),
               invocation.GetListener());
      }
    }
  }
  case TypeCheckMode::EachFile: {
    for (auto *syntaxFile :
         GetModuleSystem().GetMainModule()->GetPrimarySyntaxFiles()) {
      client(*syntaxFile, invocation.GetTypeCheckerOptions(),
             invocation.GetListener());
    }
  }
  }
}
void CompilerInstance::CompileWithSyntaxAnalysis() {
  CompileWithSyntaxAnalysis([&](syn::SyntaxFile &sf) {
    return [&](syn::SyntaxFile &sf) -> void {}(sf);
  });
}

void CompilerInstance::CompileWithSyntaxAnalysis(
    SyntaxAnalysisCallback client) {

  for (auto sourceBufferID : invocation.GetSourceBufferIDs()) {
    auto syntaxFile = SyntaxFile::Make(
        SyntaxFileKind::Library, *GetModuleSystem().GetMainModule(),
        GetSyntax().GetSyntaxContext(), sourceBufferID);

    syn::Parse(*syntaxFile, GetSyntax(), invocation.GetListener());
    assert(syntaxFile);
    client(*syntaxFile);
  }

  if (!invocation.GetCompilerOptions().GetMode().JustParse()) {
    ResolveUsings();
  }
  if (invocation.GetListener()) {
    invocation.GetListener()->OnSyntaxAnalysisCompleted(*this);
  }
}

void CompilerInstance::ResolveUsings() {
  // Resolve imports for all the source files.
  for (auto *moduleFile : GetModuleSystem().GetMainModule()->GetFiles()) {
    if (auto *syntaxFile = dyn_cast<SyntaxFile>(moduleFile))
      sem::ResolveUsings(*syntaxFile);
  }
}
void CompilerInstance::CompileWithSemanticAnalysis() {

  CompileWithSyntaxAnalysis();
  ForEachSyntaxFile([&](SyntaxFile &syntaxFile,
                        TypeCheckerOptions &typeCheckerOpts,
                        stone::TypeCheckerListener *listener) {
    sem::TypeCheck(syntaxFile, typeCheckerOpts, listener);
  });

  // TODO: FinishTypeCheck();
  if (invocation.GetListener()) {
    invocation.GetListener()->OnSemanticAnalysisCompleted(*this);
  }
}

void CompilerInstance::CompileWithSemanticAnalysis(
    SemanticAnalysisCallback client) {
  CompileWithSemanticAnalysis();
  client(*this);
}

void CompilerInstance::Compile() {

  assert(CanCompile() && "Unknown mode -- cannot continue with compile!");

  // if (GetInvocation().GetListener()) {
  //   GetInvocation().GetListener()->OnCompileStarted(*this);
  // }
  // TODO: Future CreateSyntax();

  switch (GetInvocation().GetCompilerOptions().GetMode().GetKind()) {
  case ModeKind::Parse:
    return CompileWithSyntaxAnalysis();
  case ModeKind::DumpSyntax:
    return CompileWithSyntaxAnalysis(
        [&](syn::SyntaxFile &sf) { return DumpSyntax(sf); });
  case ModeKind::TypeCheck:
    return CompileWithSemanticAnalysis();
  case ModeKind::PrintSyntax:
    return CompileWithSemanticAnalysis(
        [&](CompilerInstance &compiler) { return PrintSyntax(*this); });
  default:
    return CompileWithSemanticAnalysis(
        [&](CompilerInstance &compiler) { return CompileWithCodeGen(*this); });
  }
}
