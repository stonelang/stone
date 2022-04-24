#include "stone/Compile/Compile.h"
#include "stone/Basic/Defer.h"
#include "stone/Basic/LLVMContext.h"
#include "stone/Basic/LLVMInit.h"
#include "stone/Basic/MainExecutablePath.h"
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

  std::unique_ptr<DebugFrontendListener> debugListener;

  Frontend frontend;
  STONE_DEFER { frontend.Finish(); };

  frontend.Initialize();

  if (args.empty()) {
    frontend.GetContext().GetDiagUnit().PrintD(SrcLoc(),
                                               diag::err_no_frontend_args);
    return Finish(1);
  }

  if (listener) {
    frontend.SetListener(listener);
  } else {
    debugListener = std::make_unique<DebugFrontendListener>();
    frontend.SetListener(debugListener.get());
  }

  auto mainExecPath = llvm::sys::fs::getMainExecutable(arg0, mainAddr);
  frontend.SetMainExecutablePath(mainExecPath);

  // Setup the custom formatting to be able to handle syntax diagnostics
  auto diagFormatter = std::make_unique<SyntaxDiagnosticFormatter>();
  auto diagEmitter =
      std::make_unique<TextDiagnosticEmitter>(std::move(diagFormatter));

  TextDiagnosticListener diagListener(std::move(diagEmitter));
  frontend.GetContext().GetDiagUnit().GetDiagEngine().AddListener(diagListener);

  // Parse arguments.
  llvm::SmallVector<std::unique_ptr<llvm::MemoryBuffer>, 4>
      configurationFileBuffers;

  auto &ial = frontend.ParseArgs(args);
  if (frontend.HasError()) {
    return Finish(1);
  }
  // auto &mode = frontend.ComputeMode(ial);

  if (frontend.GetMode().IsAlien()) {
    frontend.GetContext().GetDiagUnit().PrintD(SrcLoc(), diag::err_alien_mode);
    Finish(1);
  }
  if (frontend.GetMode().IsPrintHelp()) {
    frontend.PrintHelp();
    return Finish();
  }
  if (frontend.GetMode().IsPrintVersion()) {
    frontend.PrintVersion();
    return Finish();
  }
  if (!frontend.GetMode().CanCompile()) {
    /// frontend.PrintD()
    return Finish(1);
  }
  // auto inputs = frontend.BuildInputFiles(ial);
  // if (frontend.HasError()) {
  //   return Finish(1);
  // }
  // auto sources = frontend.BuildSources(inputs);
  // if (frontend.HasError()) {
  //   return Finish(1);
  // }

  if (frontend.GetListener()) {
    frontend.GetListener()->OnCompileConfigured(frontend);
  }
  //  frontend.Compile(sources);
  //  if (frontend.HasError()) {
  //    return Finish(1);
  //  }
  return Finish();
}

static void DumpIR(Frontend &frontend, CodeGenContext &cgc,
                   IRCodeGenResult &result) {}

static void PrintIR(Frontend &frontend, CodeGenContext &cgc,
                    IRCodeGenResult &result) {}

using CompileWithGenIRCallback = llvm::function_ref<void(
    Frontend &frontend, CodeGenContext &cgc, IRCodeGenResult &result)>;

static void CompileWithGenIR(Frontend &frontend,
                             stone::ModuleSyntaxFileUnion msf,
                             CodeGenContext &cgc,
                             CompileWithGenIRCallback client) {

  switch (frontend.GetModuleOutputMode()) {
  case ModuleOutputMode::Single: {
    if (auto sf = msf.dyn_cast<SyntaxFile *>()) {
      auto result = stone::GenIR(cgc, *sf, frontend.GetContext(), nullptr);
      client(frontend, cgc, *result);
    }
    break;
  }
  case ModuleOutputMode::Whole: {
    if (auto mod = msf.get<syn::Module *>()) {
      auto result = stone::GenIR(cgc, *mod, frontend.GetContext(), nullptr);
      client(frontend, cgc, *result);
    }
    break;
  }
  default:
    stone::Panic("Unable to GenIR -- invalid IR ouput");
  }
}

static void GenModule(Frontend &frontend, CodeGenContext &cgc,
                      IRCodeGenResult &result) {}

static void CompileWithGenNative(Frontend &frontend, CodeGenContext &cgc,
                                 IRCodeGenResult &result) {

  auto targetMachine = stone::CreateTargetMachine(
      frontend.GetContext().GetDiagUnit().GetDiagEngine(),
      frontend.GetCodeGenOptions(), frontend.GetTargetOptions(),
      frontend.GetContext().GetLangOptions(),
      frontend.GetSyntax().GetSyntaxContext(), *result.GetLLVMModule());

  cgc.TakeTargetMachine(std::move(targetMachine));

  auto ComputeNativeModeKind = [&](Frontend &frontend) -> void {
    switch (frontend.GetMode().GetKind()) {
    case ModeKind::None:
    case ModeKind::EmitObject:
      frontend.GetCodeGenOptions().nativeModeKind = NativeModeKind::EmitObject;
      break;
    case ModeKind::EmitBC:
      frontend.GetCodeGenOptions().nativeModeKind = NativeModeKind::EmitBC;
      break;
    case ModeKind::EmitAssembly:
      frontend.GetCodeGenOptions().nativeModeKind =
          NativeModeKind::EmitAssembly;
      break;
    default:
      stone::Panic("Unknown Native mode kind");
    }
  };
  ComputeNativeModeKind(frontend);
  auto err = stone::GenNative(cgc, frontend.GetSyntax().GetSyntaxContext(),
                              result, nullptr);
}

static void CompileWithCodeGen(Frontend &frontend) {

  assert(frontend.GetMode().CanCodeGen());

  // We are performing some low level code generation
  CodeGenContext cgc(stone::GetLLVMContext(), frontend.GetCodeGenOptions());

  auto *mainModule = frontend.GetModuleSystem().GetMainModule();
  // switch
  // (frontend.GetFrontendOptions().moduleOutputMode)

  switch (frontend.GetMode().GetKind()) {
  case ModeKind::EmitModule:
    return CompileWithGenIR(
        frontend, mainModule, cgc,
        [&](Frontend &frontend, CodeGenContext &cgc, IRCodeGenResult &result) {
          return GenModule(frontend, cgc, result);
        });
  case ModeKind::EmitIR:
    return CompileWithGenIR(
        frontend, mainModule, cgc,
        [&](Frontend &frontend, CodeGenContext &cgc, IRCodeGenResult &result) {
          return DumpIR(frontend, cgc, result);
        });
  case ModeKind::PrintIR:
    return CompileWithGenIR(
        frontend, mainModule, cgc,
        [&](Frontend &frontend, CodeGenContext &cgc, IRCodeGenResult &result) {
          return PrintIR(frontend, cgc, result);
        });
  default:
    return CompileWithGenIR(
        frontend, mainModule, cgc,
        [&](Frontend &frontend, CodeGenContext &cgc, IRCodeGenResult &result) {
          return CompileWithGenNative(frontend, cgc, result);
        });
  }
}

static void DumpSyntax(syn::SyntaxFile &sf) {}
static void PrintSyntax(Frontend &frontend) {}

void Frontend::ForEachSyntaxFile(EachSyntaxFileCallback client) {

  switch (GetTypeCheckMode()) {
  case TypeCheckMode::WholeModule: {
    for (auto moduleFile : GetModuleSystem().GetMainModule()->GetFiles()) {
      auto *syntaxFile = dyn_cast<SyntaxFile>(moduleFile);
      if (syntaxFile) {
        client(*syntaxFile, GetTypeCheckerOptions(), GetListener());
      }
    }
  }
  case TypeCheckMode::EachFile: {
    for (auto *syntaxFile :
         GetModuleSystem().GetMainModule()->GetPrimarySyntaxFiles()) {
      client(*syntaxFile, GetTypeCheckerOptions(), GetListener());
    }
  }
  }
}
void Frontend::CompileWithSyntaxAnalysis(
    llvm::ArrayRef<FrontendUnit *> &sources) {
  CompileWithSyntaxAnalysis(sources, [&](syn::SyntaxFile &sf) {
    return [&](syn::SyntaxFile &sf) -> void {}(sf);
  });
}

void Frontend::CompileWithSyntaxAnalysis(
    llvm::ArrayRef<FrontendUnit *> &sources, SyntaxAnalysisCallback client) {

  for (auto source : sources) {
    assert(source);
    // TODO: You are not always creating a Library
    auto syntaxFile = SyntaxFile::Make(
        SyntaxFileKind::Library, *GetModuleSystem().GetMainModule(),
        GetSyntax().GetSyntaxContext(), source->GetSrcID());

    syn::Parse(*syntaxFile, GetSyntax(), GetListener());

    assert(syntaxFile);
    client(*syntaxFile);
  }

  if (!GetMode().JustParse()) {
    ResolveUsings();
  }
  if (listener) {
    listener->OnSyntaxAnalysisCompleted(*this);
  }
}

void Frontend::ResolveUsings() {
  // Resolve imports for all the source files.
  for (auto *moduleFile : GetModuleSystem().GetMainModule()->GetFiles()) {
    if (auto *syntaxFile = dyn_cast<SyntaxFile>(moduleFile))
      sem::ResolveUsings(*syntaxFile);
  }
}
void Frontend::CompileWithSemanticAnalysis(
    llvm::ArrayRef<FrontendUnit *> &sources) {

  CompileWithSyntaxAnalysis(sources);

  ForEachSyntaxFile([&](SyntaxFile &syntaxFile,
                        sem::TypeCheckerOptions &typeCheckerOpts,
                        stone::TypeCheckerListener *listener) {
    sem::TypeCheck(syntaxFile, typeCheckerOpts, listener);
  });

  // FinishTypeCheck();
  if (listener) {
    listener->OnSemanticAnalysisCompleted(*this);
  }
}

void Frontend::CompileWithSemanticAnalysis(
    llvm::ArrayRef<FrontendUnit *> &sources, SemanticAnalysisCallback client) {
  CompileWithSemanticAnalysis(sources);
  client(*this);
}

void Frontend::Compile(llvm::ArrayRef<FrontendUnit *> &sources) {

  assert(GetMode().CanCompile() &&
         "Unknown mode -- cannot continue with compile!");
  if (listener) {
    listener->OnCompileStarted(*this);
  }
  switch (GetMode().GetKind()) {
  case ModeKind::Parse:
    return CompileWithSyntaxAnalysis(sources);
  case ModeKind::DumpSyntax:
    return CompileWithSyntaxAnalysis(
        sources, [&](syn::SyntaxFile &sf) { return DumpSyntax(sf); });
  case ModeKind::TypeCheck:
    return CompileWithSemanticAnalysis(sources);
  case ModeKind::PrintSyntax:
    return CompileWithSemanticAnalysis(
        sources, [&](Frontend &frontend) { return PrintSyntax(frontend); });
  default:
    return CompileWithSemanticAnalysis(
        sources, [&](Frontend &frontend) { return CompileWithCodeGen(*this); });
  }
}
