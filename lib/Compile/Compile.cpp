#include "stone/Compile/Compile.h"
#include "stone/Basic/Defer.h"
#include "stone/Basic/FrontendDiagnostic.h"
#include "stone/Basic/LLVMContext.h"
#include "stone/Basic/LLVMInit.h"
#include "stone/Basic/MainExecutablePath.h"
#include "stone/Basic/TextDiagnosticFormatter.h"
#include "stone/Basic/TextDiagnosticListener.h"
#include "stone/Compile/DebugFrontendListener.h"
#include "stone/Compile/FrontendInstance.h"
#include "stone/Compile/FrontendListener.h"
#include "stone/Compile/TargetMachine.h"
#include "stone/Gen/CodeGenContext.h"
#include "stone/Gen/Gen.h"
#include "stone/Parse/Parse.h"
#include "stone/Sem/TypeCheck.h"
#include "stone/Session/ModeKind.h"
#include "stone/Syntax/Module.h"
#include "stone/Syntax/SyntaxDiagnosticArgument.h"

#include "llvm/IR/Module.h"

using namespace stone;
using namespace stone::syn;

/// A PrettyStackTraceEntry to print compiling information
class FrontendPrettyStackTrace : public llvm::PrettyStackTraceEntry {
  const FrontendInvocation &invocation;

public:
  FrontendPrettyStackTrace(const FrontendInvocation &invocation)
      : invocation(invocation) {}

  void print(llvm::raw_ostream &os) const override {

    //   auto effective =
    //   invocation.GetFrontendOptions().effectiveFrontendVersion; if (effective
    //   != version::Version::GetCurrentFrontendVersion()) {
    //     os << "Compiling with effective version " << effective;
    //   } else {
    //     os << "Compiling with the current frontendInstanceuage version";
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

  FrontendInstance frontendInstance;
  STONE_DEFER { frontendInstance.Finish(); };

  frontendInstance.Initialize();

  if (args.empty()) {
    frontendInstance.GetFrontendInvocation().GetContext().PrintD(
        SrcLoc(), diag::err_no_frontend_args);
    return Finish(1);
  }

  if (listener) {
    frontendInstance.SetListener(listener);
  } else {
    debugListener = std::make_unique<DebugFrontendListener>();
    frontendInstance.SetListener(debugListener.get());
  }
  auto &frontendInvocation = frontendInstance.GetFrontendInvocation();
  auto mainExecPath = llvm::sys::fs::getMainExecutable(arg0, mainAddr);
  frontendInvocation.SetMainExecutablePath(mainExecPath);

  // Setup the custom formatting to be able to handle syntax diagnostics
  auto diagFormatter = std::make_unique<SyntaxDiagnosticFormatter>();
  auto diagEmitter =
      std::make_unique<TextDiagnosticEmitter>(std::move(diagFormatter));

  TextDiagnosticListener diagListener(std::move(diagEmitter));
  frontendInvocation.GetContext().GetDiagEngine().AddListener(diagListener);

  // Parse arguments.
  llvm::SmallVector<std::unique_ptr<llvm::MemoryBuffer>, 4>
      configurationFileBuffers;

  auto &ial = frontendInvocation.ParseArgs(args);
  if (frontendInvocation.HasError()) {
    return Finish(1);
  }
  auto &mode = frontendInvocation.ComputeMode(ial);
  if (mode.IsAlien()) {
    frontendInstance.GetFrontendInvocation().GetContext().PrintD(
        SrcLoc(), diag::err_alien_mode);
    Finish(1);
  }
  if (mode.IsPrintHelp()) {
    frontendInvocation.PrintHelp(frontendInvocation.GetOpts());
    return Finish();
  }
  if (mode.IsPrintVersion()) {
    frontendInstance.PrintVersion();
    return Finish();
  }
  if (!mode.CanCompile()) {
    /// frontendInstance.PrintD()
    return Finish(1);
  }
  auto inputs = frontendInvocation.BuildInputFiles(ial);
  if (frontendInvocation.HasError()) {
    return Finish(1);
  }
  auto sources = frontendInvocation.BuildSources(inputs);
  if (frontendInvocation.HasError()) {
    return Finish(1);
  }
  if (frontendInstance.GetListener()) {
    frontendInstance.GetListener()->OnCompileConfigured(frontendInstance);
  }
  frontendInstance.Compile(sources);
  if (frontendInvocation.HasError()) {
    return Finish(1);
  }
  return Finish();
}
// static void
// CompileWithSyntaxAnalysis(FrontendUnit &source, FrontendInstance&
// frontendInstance,
//                           llvm::function_ref<void(SyntaxFile *sf)> client)
//                           {
//   // TODO: You are not always creating a Library
//   auto syntaxFile = SyntaxFile::Make(
//       SyntaxFileKind::Library,
//       *frontendInstance.GetModuleSystem().GetMainModule(),
//       frontendInstance.GetSyntax().GetSyntaxContext(), source.GetSrcID());

//   syn::ParseSyntaxFile(*syntaxFile, frontendInstance.GetSyntax());
//   client(syntaxFile);
// }

// static void CompileWithSemanticAnalysis(syn::Syntax &syntaxFile,
// FrontendInstance& frontendInstance) {
//   // assert(sf.stage == syn::SyntaxFileStage::AtImports);
//   types::TypeCheckSyntaxFile(sf,
//                              frontendInstance.GetFrontendInvocation().GetTypeCheckerOptions());
// }

// static void
// CompileWithAnalysis(llvm::ArrayRef<FrontendUnit *> sources, FrontendInstance&
// frontendInstance,
//                     llvm::function_ref<void(FrontendInstance
//                     &frontendInstance)> client) {
//   for (auto source : sources) {
//     assert(source);
//     CompileWithAnalysis(*source);
//   }
//   client(frontendInstance);

// if (frontendInstance.GetFrontendInvocation().GetMode().JustParse()) {
//   return;
// }
// if (frontendInstance.GetFrontendInvocation().GetTypeCheckMode() ==
//     types::TypeCheckMode::WholeModule) {
//   TypeCheckModule(nullptr /*TODO: get module*/);
// }
// if (frontendInstance.GetFrontendInvocation().GetMode().JustTypeCheck()) {
//   // Do some things
//   return;
// }
//}

// static void CompileWithAnalysis(FrontendUnit &source, FrontendInstance&
// frontendInstance) {

//   switch (frontendInstance.GetFrontendInvocation().GetMode().GetKind()) {
//   case ModeKind::Parse:
//     return CompileWithSyntaxAnalysis(source, frontendInstance,
//     [&](FrontendInstance &frontendInstance)
//     {
//       return [&](syn::SyntaxFile &sf) -> void {}(sf, frontendInstance);
//     });
//   case ModeKind::DumpSyntax:
//     return CompileWithSyntaxAnalysis(
//         source, frontendInstance, [&](syn::SyntaxFile &sf) { return
//         DumpSyntax(sf, frontendInstance);
//         });

//   case ModeKind::TypeCheck:
//     return CompileWithSyntaxAnalysis(source, frontendInstance,
//     [&](syn::SyntaxFile &sf)
//     {
//       return CompileWithSemanticAnalysis(sf, frontendInstance);
//     });

//   case ModeKind::PrintSyntax:
//     return CompileWithSyntaxAnalysis(
//         source, frontendInstance, [&](syn::SyntaxFile &sf) { return
//         PrintSyntax(sf, frontendInstance); });
//   }
// Add to module
// TODO: May not need this because we are going to add it to the main module.
// source.SetSyntaxFile(syntaxFile);

// if (frontendInstance.GetFrontendInvocation().GetMode().IsParse()) {
//   return;
// }
// if (frontendInstance.GetFrontendInvocation().GetMode().IsDumpSyntax()) {
//   // frontendInstance.DumpSyntax(syntaxFile);
//   return;
// }
// if (frontendInstance.GetFrontendInvocation().GetTypeCheckMode() ==
//     types::TypeCheckMode::EachFile) {
//   TypeCheckSyntaxFile(*syntaxFile);
// }

//}

// void CodeAnalysis::TypeCheckSyntaxFile(SyntaxFile &sf) {
//   assert(sf.stage == syn::SyntaxFileStage::AtImports);
//   types::TypeCheckSyntaxFile(sf,
//                              frontendInstance.GetFrontendInvocation().GetTypeCheckerOptions());
// }
// /// Perform type-checking on the entire module
// void CodeAnalysis::TypeCheckModule(syn::Module *mod) {
//   assert(mod && "Null 'syn::Module'");
//   for (auto mf : mod->GetFiles()) {
//     if (auto sf = llvm::dyn_cast<SyntaxFile>(mf)) {
//       TypeCheckSyntaxFile(*sf);
//     }
//   }
// }

// void CompilePostSemanticAnalysis(FrontendInstance& frontendInstance) {

//   assert(frontendInstance.GetFrontendInvocation().CanCodeGen());
//   assert(ca.GetCodeAnalysisResultKind() == CodeAnalysisResultKind::None);

//   // We are performing some low leverl code generation
//   CodeGenContext cgc(stone::GetLLVMContext(),
//                      frontendInstance.GetFrontendInvocation().GetCodeGenOptions());

//   switch (ca.GetCodeAnalysisResultKind()) {
//   case CodeAnalysisResultKind::SyntaxFile: {
//     GenerateIR(*ca.GetSyntaxFile(), cgc);
//     break;
//   }
//   case CodeAnalysisResultKind::Module: {
//     GenerateIR(*ca.GetModulde(), cgc);
//     break;
//   }
//   default:
//     stone::Panic("Unknown code analysis result");
//   }

//   CodeOptimization codeOptimization(frontendInstance);
//   codeOptimization.Optimize(/*cgc.GetLLVMModule()*/);

//   if (frontendInstance.GetFrontendInvocation().GetMode().IsEmitIR()) {
//     // EmitIR()
//     return;
//   }

//   if (frontendInstance.GetFrontendInvocation().GetMode().IsNone() ||
//       frontendInstance.GetFrontendInvocation().GetMode().IsEmitObject()) {
//     // GenObject(srcID, llvmMod, cgc);
//     return;
//   }
// }
// llvm::Module *CodeGeneration::GenerateIR(syn::SyntaxFile &sf,
//                                          CodeGenContext &cc) {
//   return nullptr;
// }

// llvm::Module *CodeGeneration::GenerateIR(syn::Module &mod, CodeGenContext
// &cc) {
//   return nullptr;
// }

// void CodeGeneration::GenerateObject(const unsigned srcID, llvm::Module
// *mod,
//                                     CodeGenContext &cc) {
//   /// TODO: This is the only time we should perform a lookup
//   // auto outputFile =
//   frontendInstance.GetFrontendInvocation().ComputeOutputFile(srcID);
//   // auto result GenObject(cgc GetSyntaxContext(), outputFile.get());
// }

static void DumpIR(FrontendInstance &frontendInstance, CodeGenContext &cgc,
                   IRCodeGenResult &result) {}

using CompileWithGenIRCallback =
    llvm::function_ref<void(FrontendInstance &frontendInstance,
                            CodeGenContext &cgc, IRCodeGenResult &result)>;

static void CompileWithGenIR(FrontendInstance &frontendInstance,
                             stone::ModuleSyntaxFileUnion msf,
                             CodeGenContext &cgc,
                             CompileWithGenIRCallback client) {

  switch (frontendInstance.GetFrontendInvocation().GetModuleOutputMode()) {
  case ModuleOutputMode::Single: {
    if (auto sf = msf.dyn_cast<SyntaxFile *>()) {
      auto result = stone::GenIR(
          cgc, *sf, frontendInstance.GetFrontendInvocation().GetContext(),
          nullptr);
      client(frontendInstance, cgc, *result);
    }
    break;
  }
  case ModuleOutputMode::Whole: {
    if (auto mod = msf.get<syn::Module *>()) {
      auto result = stone::GenIR(
          cgc, *mod, frontendInstance.GetFrontendInvocation().GetContext(),
          nullptr);
      client(frontendInstance, cgc, *result);
    }
    break;
  }
  default:
    stone::Panic("Unable to GenIR -- invalid IR ouput");
  }
}

static void GenModule(FrontendInstance &frontendInstance, CodeGenContext &cgc,
                      IRCodeGenResult &result) {}

static void CompileWithGenNative(FrontendInstance &frontendInstance,
                                 CodeGenContext &cgc, IRCodeGenResult &result) {

  auto targetMachine = stone::CreateTargetMachine(
      frontendInstance.GetFrontendInvocation().GetContext().GetDiagEngine(),
      frontendInstance.GetFrontendInvocation().GetCodeGenOptions(),
      frontendInstance.GetFrontendInvocation().GetFrontendOptions(),
      frontendInstance.GetSyntax().GetSyntaxContext(), *result.GetLLVMModule());

  cgc.TakeTargetMachine(std::move(targetMachine));

  auto ComputeNativeModeKind = [&](FrontendInstance &frontendInstance) -> void {
    switch (frontendInstance.GetFrontendInvocation().GetMode().GetKind()) {
    case ModeKind::None:
    case ModeKind::EmitObject:
      frontendInstance.GetFrontendInvocation()
          .GetCodeGenOptions()
          .nativeModeKind = NativeModeKind::EmitObject;
      break;
    case ModeKind::EmitBC:
      frontendInstance.GetFrontendInvocation()
          .GetCodeGenOptions()
          .nativeModeKind = NativeModeKind::EmitBC;
      break;
    case ModeKind::EmitAssembly:
      frontendInstance.GetFrontendInvocation()
          .GetCodeGenOptions()
          .nativeModeKind = NativeModeKind::EmitAssembly;
      break;
    default:
      stone::Panic("Unknown Native mode kind");
    }
  };
  ComputeNativeModeKind(frontendInstance);
  auto status = stone::GenNative(
      cgc, frontendInstance.GetSyntax().GetSyntaxContext(), result, nullptr);
}

static void CompileWithCodeGen(FrontendInstance &frontendInstance) {

  assert(frontendInstance.GetFrontendInvocation().CanCodeGen());

  // We are performing some low level code generation
  CodeGenContext cgc(
      stone::GetLLVMContext(),
      frontendInstance.GetFrontendInvocation().GetCodeGenOptions());

  auto *mainModule = frontendInstance.GetModuleSystem().GetMainModule();
  // switch
  // (frontendInstance.GetFrontendInvocation().GetFrontendOptions().moduleOutputMode)

  switch (frontendInstance.GetFrontendInvocation().GetMode().GetKind()) {
  case ModeKind::EmitModule:
    return CompileWithGenIR(frontendInstance, mainModule, cgc,
                            [&](FrontendInstance &frontendInstance,
                                CodeGenContext &cgc, IRCodeGenResult &result) {
                              return GenModule(frontendInstance, cgc, result);
                            });
  case ModeKind::EmitIR:
    return CompileWithGenIR(frontendInstance, mainModule, cgc,
                            [&](FrontendInstance &frontendInstance,
                                CodeGenContext &cgc, IRCodeGenResult &result) {
                              return DumpIR(frontendInstance, cgc, result);
                            });
  default:
    return CompileWithGenIR(frontendInstance, mainModule, cgc,
                            [&](FrontendInstance &frontendInstance,
                                CodeGenContext &cgc, IRCodeGenResult &result) {
                              return CompileWithGenNative(frontendInstance, cgc,
                                                          result);
                            });
  }
}

static void DumpSyntax(syn::SyntaxFile &sf) {}
static void PrintSyntax(FrontendInstance &frontendInstance) {}

void FrontendInstance::Compile(llvm::ArrayRef<FrontendUnit *> &sources) {

  assert(GetFrontendInvocation().GetMode().CanCompile() &&
         "Unknown mode -- cannot continue with compile!");
  if (listener) {
    listener->OnCompileStarted(*this);
  }
  switch (GetFrontendInvocation().GetMode().GetKind()) {
  case ModeKind::Parse:
    return CompileWithSyntaxAnalysis(sources);
  case ModeKind::DumpSyntax:
    return CompileWithSyntaxAnalysis(
        sources, [&](syn::SyntaxFile &sf) { return DumpSyntax(sf); });
  case ModeKind::TypeCheck:
    return CompileWithSemanticAnalysis(sources);
  case ModeKind::PrintSyntax:
    return CompileWithSemanticAnalysis(sources,
                                       [&](FrontendInstance &frontendInstance) {
                                         return PrintSyntax(frontendInstance);
                                       });
  default:
    return CompileWithSemanticAnalysis(sources,
                                       [&](FrontendInstance &frontendInstance) {
                                         return CompileWithCodeGen(*this);
                                       });
  }
}
