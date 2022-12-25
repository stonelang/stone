#include "stone/Compile/Compile.h"
#include "stone/Basic/Defer.h"
#include "stone/Basic/LLVMContext.h"
#include "stone/Basic/LLVMInit.h"
#include "stone/Basic/MainExecutablePath.h"
#include "stone/CodeCompletionListener.h"
#include "stone/Compile/CompilerInstance.h"
#include "stone/Compile/CompilerInvocation.h"
#include "stone/Diag/CompilerDiagnostic.h"
#include "stone/Diag/TextDiagnosticFormatter.h"
#include "stone/Diag/TextDiagnosticListener.h"
#include "stone/Gen/CodeGenContext.h"
#include "stone/Gen/Gen.h"
#include "stone/Parse/Parse.h"
#include "stone/Sem/TypeCheck.h"
#include "stone/Session/ModeKind.h"
#include "stone/Syntax/Module.h"
#include "stone/Syntax/SyntaxDiagnosticArgument.h"

#include "clang/Basic/TargetInfo.h"

#include "llvm/IR/Module.h"
#include "llvm/Support/Casting.h"

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

  auto Finish = [&](Error err = Error()) -> int { return err.GetFlag(); };

  auto programPath = llvm::sys::fs::getMainExecutable(arg0, mainAddr);
  auto programName = file::GetStem(programPath);

  CompilerInvocation invocation(programName, programPath, listener);
  STONE_DEFER { invocation.Finish(); };

  if (args.empty()) {
    invocation.GetLangContext().GetDiagUnit().PrintD(SrcLoc(),
                                                     diag::err_no_input_files);
    return Finish(Error(true));
  }
  // We setup clang now -- this just loads the instance.
  invocation.SetupClang(args, arg0);

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
    return Finish(Error(true));
  }
  if (invocation.HasError()) {
    return Finish(Error(true));
  }
  if (invocation.ComputeOptions(*ial).Has()) {
    return Finish(Error(true));
  }
  if (invocation.GetCompilerOptions().GetMode().IsAlien()) {
    invocation.GetLangContext().GetDiagUnit().PrintD(SrcLoc(),
                                                     diag::err_alien_mode);
    Finish(Error(true));
  }
  if (invocation.GetCompilerOptions().GetMode().IsPrintHelp()) {
    // TODO: invocation.PrintHelp(invocation.GetOpts());
    return Finish();
  }
  if (invocation.GetCompilerOptions().GetMode().IsPrintVersion()) {
    invocation.PrintVersion();
    return Finish(Error(true));
  }
  if (!invocation.GetCompilerOptions().GetMode().CanCompile()) {
    /// invocation.PrintD()
    return Finish();
  }
  if (invocation.GetListener()) {
    invocation.GetListener()->OnCompileConfigured(invocation);
  }
  if (invocation.CreateSourceBuffers().Has()) {
    return Finish();
  }

  CompilerInstance compiler(invocation);
  auto status = compiler.Compile();

  if (status.IsError() || invocation.HasError()) {
    return Finish(Error(true));
  }
  return Finish();
}

static CompileStatus DumpIR(CompilerInstance &compiler, CodeGenContext &cgc) {
  CompileStatus::MakeSuccess();
}

static CompileStatus PrintIR(CompilerInstance &compiler, CodeGenContext &cgc) {
  CompileStatus::MakeSuccess();
}

CompileStatus
CompilerInstance::CompileWithGenIR(CodeGenContext &cgc,
                                   IRCodeGenCompletedCallback notifiy) {
  CompileStatus status;
  const auto &compilerInvocation = GetInvocation();
  const CompilerOptions &compilerOpts = compilerInvocation.GetCompilerOptions();

  auto GenSyntaxFileOrWholeModule =
      [&](const PrimaryFileSpecificPaths primarySpecificPaths,
          stone::ModuleSyntaxFileUnion msf) -> CompileStatus {
    if (auto syntaxFile = CastToSyntaxFile(msf)) {
      stone::GenIR(cgc, *syntaxFile, primarySpecificPaths);
      notifiy(*this, cgc);
    } else if (auto moduleDecl = CastToModuleDecl(msf)) {
      stone::GenIR(cgc, *moduleDecl, primarySpecificPaths);
      return notifiy(*this, cgc);
    }
    return CompileStatus::MakeError();
  };

  if (!compilerOpts.GetInputsAndOutputs().HasPrimaryInputs()) {
    auto *mainModule = GetModuleSystem().GetMainModule();
    const PrimaryFileSpecificPaths primaryFileSpecificPaths =
        GetPrimaryFileSpecificPathsForWholeModuleOptimizationMode();
    return GenSyntaxFileOrWholeModule(primaryFileSpecificPaths, mainModule);
  }
  if (!GetPrimarySyntaxFiles().empty()) {
    // bool result = false;
    for (auto *primarySyntaxFile : GetPrimarySyntaxFiles()) {
      const PrimaryFileSpecificPaths primaryFileSpecificPaths =
          GetPrimaryFileSpecificPathsForSyntaxFile(*primarySyntaxFile);
      auto status = GenSyntaxFileOrWholeModule(primaryFileSpecificPaths,
                                               primarySyntaxFile);

      if (status.IsError()) {
        break;
      } else {
        notifiy(*this, cgc);
      }
    }
  }

  status.SetIsError();
  return status;
}
static CompileStatus GenModule(CompilerInstance &compiler,
                               CodeGenContext &cgc) {
  return CompileStatus::MakeSuccess();
}

CompileStatus CompilerInstance::CompileWithGenNative(CodeGenContext &cgc) {

  auto ComputeNativeModeKind = [&]() -> void {
    switch (GetMode().GetKind()) {
    case ModeKind::None:
    case ModeKind::EmitObject:
      GetInvocation().GetCodeGenOptions().nativeModeKind =
          NativeModeKind::EmitObject;
      break;
    case ModeKind::EmitBC:
      GetInvocation().GetCodeGenOptions().nativeModeKind =
          NativeModeKind::EmitBC;
      break;
    case ModeKind::EmitAssembly:
      GetInvocation().GetCodeGenOptions().nativeModeKind =
          NativeModeKind::EmitAssembly;
      break;
    default:
      stone::Panic("Unknown Native mode kind");
    }
  };
  ComputeNativeModeKind();
  stone::GenNative(cgc, GetSyntaxContext(), llvm::StringRef(),
                   GetInvocation().GetListener());

  return CompileStatus::MakeSuccess();
}

CompileStatus CompilerInstance::CompileWithCodeGen() {

  assert(GetMode().CanCodeGen() && "Mode does not support code gen");

  // We are performing some low level code generation
  CodeGenContext cgc(
      stone::GetLLVMContext(), GetInvocation().GetCodeGenOptions(),
      GetInvocation().GetModuleOptions(), GetInvocation().GetTargetOptions(),
      GetInvocation().GetLangContext(), GetInvocation().GetClangContext());

  // auto *Module = IGM.getModule();
  // assert(Module && "Expected llvm:Module for IR generation!");

  // Module->setTargetTriple(IGM.Triple.str());

  // // Set the module's string representation.
  // Module->setDataLayout(IGM.DataLayout.getStringRepresentation());

  // cgc.Initialize();

  // clang::TargetInfo &targetInfo =
  //     GetInvocation().GetClangContext().GetInstance().getTarget();

  // // Setup the empty module
  // cgc.GetLLVMModule().setTargetTriple(targetInfo.getTriple().getTriple());
  // cgc.GetLLVMModule().setDataLayout(targetInfo.getDataLayoutString());

  // const auto &sdkVersion = targetInfo.getSDKVersion();

  // if (!sdkVersion.empty()) {
  //   cgc.GetLLVMModule().setSDKVersion(sdkVersion);
  // }

  // if (const auto *tvt = targetInfo.getDarwinTargetVariantTriple()) {
  //   cgc.GetModule().setDarwinTargetVariantTriple(tvt->getTriple());
  // }

  // if (auto TVSDKVersion = targetInfo.getDarwinTargetVariantSDKVersion()) {
  //   cgc.GetModule().setDarwinTargetVariantSDKVersion(*TVSDKVersion);
  // }

  // auto targetMachine = stone::CreateTargetMachine(
  //     compiler.GetInvocation().GetLangContext().GetDiagUnit().GetDiagEngine(),
  //     compiler.GetInvocation().GetCodeGenOptions(),
  //     compiler.GetInvocation().GetTargetOptions(),
  //     compiler.GetInvocation().GetLangContext().GetLangOptions(),
  //     compiler.GetSyntaxContext());

  // switch
  // (invocation.GetCompilerOptions().moduleOutputMode)

  switch (GetMode().GetKind()) {
  case ModeKind::EmitModule:
    return CompileWithGenIR(
        cgc, [&](CompilerInstance &compiler, CodeGenContext &cgc) {
          return GenModule(*this, cgc);
        });
  case ModeKind::EmitIR:
    return CompileWithGenIR(
        cgc, [&](CompilerInstance &compiler, CodeGenContext &cgc) {
          return DumpIR(*this, cgc);
        });
  case ModeKind::PrintIR:
    return CompileWithGenIR(
        cgc, [&](CompilerInstance &compiler, CodeGenContext &cgc) {
          return PrintIR(*this, cgc);
        });
  default:
    return CompileWithGenIR(
        cgc, [&](CompilerInstance &compiler, CodeGenContext &cgc) {
          return CompileWithGenNative(cgc);
        });
  }
}

static CompileStatus DumpSyntax(CompilerInstance &compiler, syn::SyntaxFile &sf) {
  return CompileStatus::MakeSuccess();
}

static CompileStatus PrintSyntax(CompilerInstance &compiler) {
  return CompileStatus::MakeSuccess();
}

CompileStatus CompilerInstance::CompileWithParsing() {
  return CompileWithParsing(
      [&](syn::SyntaxFile &) { return CompileStatus::MakeSuccess(); });
}

CompileStatus
CompilerInstance::CompileWithParsing(ParsingCompletedCallback notifiy) {

  for (auto moduleFile : GetModuleSystem().GetMainModule()->GetFiles()) {
    if (auto *syntaxFile = llvm::dyn_cast<syn::SyntaxFile>(moduleFile)) {
      syn::Parse(*syntaxFile, GetSyntaxContext(), invocation.GetListener());
      notifiy(*syntaxFile);
    }
  }

  if (!GetMode().JustParse()) {
    ResolveImports();
  }
  if (invocation.GetListener()) {
    invocation.GetListener()->OnSyntaxAnalysisCompleted(*this);
  }
  return CompileStatus::MakeSuccess();
}

CompileStatus CompilerInstance::CompileWithTypeChecking() {
  return CompileWithTypeChecking(
      [&](CompilerInstance &) { return CompileStatus::MakeSuccess(); });
}

CompileStatus CompilerInstance::CompileWithTypeChecking(
    TypeCheckingCompletedCallback notifiy) {

  auto status = CompileWithParsing();
  if (status.IsError()) {
    return status;
  }
  ForEachSyntaxFile([&](SyntaxFile &syntaxFile,
                        TypeCheckerOptions &typeCheckerOpts,
                        stone::TypeCheckerListener *listener) {
    sem::TypeCheck(syntaxFile, typeCheckerOpts, listener);
  });

  // TODO: FinishTypeCheck();
  if (invocation.GetListener()) {
    invocation.GetListener()->OnSemanticAnalysisCompleted(*this);
  }
  return notifiy(*this);
}
CompileStatus CompilerInstance::Compile() {

  assert(CanCompile() && "Unknown mode -- cannot continue with compile!");

  if (GetInvocation().GetListener()) {
    GetInvocation().GetListener()->OnCompileStarted(*this);
  }
  CompileStatus status;
  switch (GetMode().GetKind()) {
  case ModeKind::Parse:
    status = CompileWithParsing();
    break;
  case ModeKind::DumpSyntax:
    status =
        CompileWithParsing([&](syn::SyntaxFile &sf) { return DumpSyntax(*this, sf); });
    break;
  case ModeKind::TypeCheck:
    status = CompileWithTypeChecking();
    break;
  case ModeKind::PrintSyntax:
    status = CompileWithTypeChecking(
        [&](CompilerInstance &compiler) { return PrintSyntax(*this); });
    break;
  default:
    status = CompileWithTypeChecking(
        [&](CompilerInstance &compiler) { return CompileWithCodeGen(); });
    break;
  }
  return status;
}
