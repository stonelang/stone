#include "stone/Compile/Compile.h"
#include "stone/Basic/Defer.h"
#include "stone/Basic/LLVMContext.h"
#include "stone/Basic/LLVMInit.h"
#include "stone/Basic/MainExecutablePath.h"
#include "stone/Compile/CompilerInstance.h"
#include "stone/Compile/DebugFrontendListener.h"
#include "stone/Compile/Frontend.h"
#include "stone/Compile/FrontendListener.h"
#include "stone/Compile/TargetMachine.h"
#include "stone/Diag/FrontendDiagnostic.h"
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
class FrontendPrettyStackTrace : public llvm::PrettyStackTraceEntry {
  const Frontend &frontend;

public:
  FrontendPrettyStackTrace(const Frontend &frontend) : frontend(frontend) {}

  void print(llvm::raw_ostream &os) const override {

    //   auto effective =
    //   invocation.GetFrontendOptions().effectiveFrontendVersion; if (effective
    //   != version::Version::GetCurrentFrontendVersion()) {
    //     os << "Compiling with effective version " << effective;
    //   } else {
    //     os << "Compiling with the current frontenduage version";
    //   }
    //   if (Invocation.GetFrontendOptions().allowModuleWithCompilerErrors) {
    //     os << " while allowing modules with compiler errors";
    //   }
    //   os << "\n";
  }
};

int stone::Compile(llvm::ArrayRef<const char *> args, const char *arg0,
                   void *mainAddr, FrontendListener *listener) {
  llvm::PrettyStackTraceString crashInfo("Compile construction...");
  FINISH_LLVM_INIT();

  auto Finish = [&](int status = 0) -> int {
    int err = 1;
    return status ? status : err;
  };

  auto programPath = llvm::sys::fs::getMainExecutable(arg0, mainAddr);
  auto programName = file::GetStem(programPath);

  Frontend frontend(programName, programPath, listener);
  STONE_DEFER { frontend.Finish(); };

  if (args.empty()) {
    frontend.GetContext().GetDiagUnit().PrintD(SrcLoc(),
                                               diag::err_no_input_files);
    return Finish(1);
  }
  // Setup the custom formatting to be able to handle syntax diagnostics
  auto diagFormatter = std::make_unique<SyntaxDiagnosticFormatter>();
  auto diagEmitter =
      std::make_unique<TextDiagnosticEmitter>(std::move(diagFormatter));

  TextDiagnosticListener diagListener(std::move(diagEmitter));
  frontend.GetContext().GetDiagUnit().GetDiagEngine().AddListener(diagListener);

  // Parse arguments.
  llvm::SmallVector<std::unique_ptr<llvm::MemoryBuffer>, 4>
      configurationFileBuffers;

  auto ial = frontend.ParseArgs(args);
  if (!ial) {
    return Finish(1);
  }
  if (frontend.HasError()) {
    return Finish(1);
  }
  if (frontend.ComputeOptions(*ial).Has()) {
    return Finish(1);
  }
  if (frontend.GetFrontendOptions().GetMode().IsAlien()) {
    frontend.GetContext().GetDiagUnit().PrintD(SrcLoc(), diag::err_alien_mode);
    Finish(1);
  }
  if (frontend.GetFrontendOptions().GetMode().IsPrintHelp()) {
    // TODO: frontend.PrintHelp(frontend.GetOpts());
    return Finish();
  }
  if (frontend.GetFrontendOptions().GetMode().IsPrintVersion()) {
    frontend.PrintVersion();
    return Finish();
  }
  if (!frontend.GetFrontendOptions().GetMode().CanCompile()) {
    /// frontend.PrintD()
    return Finish(1);
  }
  if (frontend.GetListener()) {
    frontend.GetListener()->OnCompileConfigured(frontend);
  }
  if (frontend.CreateSourceBuffers().Has()) {
    return Finish(1);
  }

  CompilerInstance compiler(frontend);
  compiler.Compile();

  if (frontend.HasError()) {
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
      auto result =
          stone::GenIR(cgc, *sf, compiler.GetFrontend().GetContext(), nullptr);
      client(compiler, cgc, *result);
    }
    break;
  }
  case ModuleOutputMode::Whole: {
    if (auto mod = msf.get<syn::Module *>()) {
      auto result =
          stone::GenIR(cgc, *mod, compiler.GetFrontend().GetContext(), nullptr);
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

static void CompileWithGenNative(CompilerInstance &compiler, CodeGenContext &cgc,
                                 IRCodeGenResult &result) {

  auto targetMachine = stone::CreateTargetMachine(
      compiler.GetFrontend().GetContext().GetDiagUnit().GetDiagEngine(),
      compiler.GetFrontend().GetCodeGenOptions(),
      compiler.GetFrontend().GetTargetOptions(),
      compiler.GetFrontend().GetContext().GetLangOptions(),
      compiler.GetSyntax().GetSyntaxContext(), *result.GetLLVMModule());

  cgc.TakeTargetMachine(std::move(targetMachine));

  auto ComputeNativeModeKind = [&](CompilerInstance &compiler) -> void {
    switch (compiler.GetFrontend().GetFrontendOptions().GetMode().GetKind()) {
    case ModeKind::None:
    case ModeKind::EmitObject:
      compiler.GetFrontend().GetCodeGenOptions().nativeModeKind =
          NativeModeKind::EmitObject;
      break;
    case ModeKind::EmitBC:
      compiler.GetFrontend().GetCodeGenOptions().nativeModeKind =
          NativeModeKind::EmitBC;
      break;
    case ModeKind::EmitAssembly:
      compiler.GetFrontend().GetCodeGenOptions().nativeModeKind =
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

  assert(compiler.GetFrontend().GetFrontendOptions().GetMode().CanCodeGen());

  // We are performing some low level code generation
  CodeGenContext cgc(stone::GetLLVMContext(),
                     compiler.GetFrontend().GetCodeGenOptions(),
                     compiler.GetFrontend().GetContext());

  auto *mainModule = compiler.GetModuleSystem().GetMainModule();
  // switch
  // (frontend.GetFrontendOptions().moduleOutputMode)

  switch (compiler.GetFrontend().GetFrontendOptions().GetMode().GetKind()) {
  case ModeKind::EmitModule:
    return CompileWithGenIR(
        compiler, mainModule, cgc,
        [&](CompilerInstance &compiler, CodeGenContext &cgc, IRCodeGenResult &result) {
          return GenModule(compiler, cgc, result);
        });
  case ModeKind::EmitIR:
    return CompileWithGenIR(
        compiler, mainModule, cgc,
        [&](CompilerInstance &compiler, CodeGenContext &cgc, IRCodeGenResult &result) {
          return DumpIR(compiler, cgc, result);
        });
  case ModeKind::PrintIR:
    return CompileWithGenIR(
        compiler, mainModule, cgc,
        [&](CompilerInstance &compiler, CodeGenContext &cgc, IRCodeGenResult &result) {
          return PrintIR(compiler, cgc, result);
        });
  default:
    return CompileWithGenIR(
        compiler, mainModule, cgc,
        [&](CompilerInstance &compiler, CodeGenContext &cgc, IRCodeGenResult &result) {
          return CompileWithGenNative(compiler, cgc, result);
        });
  }
}

static void DumpSyntax(syn::SyntaxFile &sf) {}

static void PrintSyntax(CompilerInstance &compiler) {}

void CompilerInstance::ForEachSyntaxFile(EachSyntaxFileCallback client) {

  switch (frontend.GetTypeCheckMode()) {
  case TypeCheckMode::WholeModule: {
    for (auto moduleFile : GetModuleSystem().GetMainModule()->GetFiles()) {
      auto *syntaxFile = dyn_cast<SyntaxFile>(moduleFile);
      if (syntaxFile) {
        client(*syntaxFile, frontend.GetTypeCheckerOptions(),
               frontend.GetListener());
      }
    }
  }
  case TypeCheckMode::EachFile: {
    for (auto *syntaxFile :
         GetModuleSystem().GetMainModule()->GetPrimarySyntaxFiles()) {
      client(*syntaxFile, frontend.GetTypeCheckerOptions(),
             frontend.GetListener());
    }
  }
  }
}
void CompilerInstance::CompileWithSyntaxAnalysis() {
  CompileWithSyntaxAnalysis([&](syn::SyntaxFile &sf) {
    return [&](syn::SyntaxFile &sf) -> void {}(sf);
  });
}

void CompilerInstance::CompileWithSyntaxAnalysis(SyntaxAnalysisCallback client) {

  for (auto sourceBufferID : frontend.GetSourceBufferIDs()) {
    auto syntaxFile = SyntaxFile::Make(
        SyntaxFileKind::Library, *GetModuleSystem().GetMainModule(),
        GetSyntax().GetSyntaxContext(), sourceBufferID);

    syn::Parse(*syntaxFile, GetSyntax(), frontend.GetListener());
    assert(syntaxFile);
    client(*syntaxFile);
  }

  if (!frontend.GetFrontendOptions().GetMode().JustParse()) {
    ResolveUsings();
  }
  if (frontend.GetListener()) {
    frontend.GetListener()->OnSyntaxAnalysisCompleted(frontend);
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
                        sem::TypeCheckerOptions &typeCheckerOpts,
                        stone::TypeCheckerListener *listener) {
    sem::TypeCheck(syntaxFile, typeCheckerOpts, listener);
  });

  // TODO: FinishTypeCheck();
  if (frontend.GetListener()) {
    frontend.GetListener()->OnSemanticAnalysisCompleted(frontend);
  }
}

void CompilerInstance::CompileWithSemanticAnalysis(SemanticAnalysisCallback client) {
  CompileWithSemanticAnalysis();
  client(*this);
}

void CompilerInstance::Compile() {

  assert(CanCompile() && "Unknown mode -- cannot continue with compile!");

  // if (GetFrontend().GetListener()) {
  //   GetFrontend().GetListener()->OnCompileStarted(*this);
  // }
  // TODO: Future CreateSyntax();

  switch (GetFrontend().GetFrontendOptions().GetMode().GetKind()) {
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
