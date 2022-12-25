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
#include "stone/Sem/ImportResolution.h"
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
  CompileStatus status = compiler.Compile();

  if (invocation.HasError()) {
    return Finish(1);
  }
  return Finish();
}

static CompileStatus DumpIR(CompilerInstance &compiler, CodeGenContext &cgc) {
  CompileStatus::MakeSuccess();
}

static CompileStatus PrintIR(CompilerInstance &compiler, CodeGenContext &cgc) {
  CompileStatus::MakeSuccess();
}

static CompileStatus CompileWithGenIR(CompilerInstance &compilerInstance,
                                      stone::ModuleSyntaxFileUnion msf,
                                      CodeGenContext &cgc,
                                      IRCodeGenCompletedCallback fn) {
  CompileStatus status;
  const auto &compilerInvocation = compilerInstance.GetInvocation();
  const CompilerOptions &compilerOpts = compilerInvocation.GetCompilerOptions();

  //  // TODO: Move to CompilerInstance
  // auto targetMachine = stone::CreateTargetMachine(
  //     compiler.GetInvocation().GetLangContext().GetDiagUnit().GetDiagEngine(),
  //     compiler.GetInvocation().GetCodeGenOptions(),
  //     compiler.GetInvocation().GetTargetOptions(),
  //     compiler.GetInvocation().GetLangContext().GetLangOptions(),
  //     compiler.GetSyntaxContext());

  // switch (compiler.GetModuleOutputMode()) {
  // case ModuleOutputMode::Single: {
  //   if (auto sf = msf.dyn_cast<SyntaxFile *>()) {
  //     stone::GenIR(cgc, *sf, compiler.GetInvocation().GetLangContext(),
  //                  nullptr);
  //     status |= fn(compiler, cgc);
  //   }
  //   return status;
  // }
  // case ModuleOutputMode::Whole: {
  //   if (auto mod = msf.get<syn::ModuleDecl *>()) {
  //     stone::GenIR(cgc, *mod, compiler.GetInvocation().GetLangContext(),
  //                  nullptr);
  //     status |= fn(compiler, cgc);
  //   }
  //   return status;
  // }
  // default:
  //   stone::Panic("Unable to GenIR -- invalid IR ouput");
  // }

  auto *mainModule = compilerInstance.GetModuleSystem().GetMainModule();
  if (!compilerOpts.GetInputsAndOutputs().HasPrimaryInputs()) {

    // If there are no primary inputs the compiler is in WMO mode and builds one
    // SILModule for the entire module.
    const PrimaryFileSpecificPaths primaryFileSpecificPaths =
        compilerInstance
            .GetPrimaryFileSpecificPathsForWholeModuleOptimizationMode();

    // SILOptions SILOpts = getSILOptions(PSPs);
    // IRGenOptions irgenOpts = Invocation.getIRGenOptions();
    // auto SM = performASTLowering(mod, Instance.getSILTypes(), SILOpts,
    //                              &irgenOpts);
    // return performCompileStepsPostSILGen(Instance, std::move(SM), mod, PSPs,
    //                                      ReturnValue, observer);
  }
  // If there are primary source files, build a separate SILModule for
  // each source file, and run the remaining SILOpt-Serialize-IRGen-LLVM
  // once for each such input.
  if (!compilerInstance.GetPrimarySyntaxFiles().empty()) {
    // bool result = false;
    // for (auto *PrimaryFile : Instance.getPrimarySourceFiles()) {
    //   const PrimarySpecificPaths PSPs =
    //       Instance.getPrimarySpecificPathsForSourceFile(*PrimaryFile);
    //   SILOptions SILOpts = getSILOptions(PSPs);
    // IRGenOptions irgenOpts = Invocation.getIRGenOptions();
    //   auto SM = performASTLowering(*PrimaryFile, Instance.getSILTypes(),
    //                                SILOpts, &irgenOpts);
    //   result |= performCompileStepsPostSILGen(Instance, std::move(SM),
    //                                           PrimaryFile, PSPs, ReturnValue,
    //                                           observer);
    //}

    // return result;
  }

  // if (auto sf = msf.dyn_cast<SyntaxFile *>()) {
  //   stone::GenIR(cgc, *sf, compiler.GetInvocation().GetLangContext(),
  //                llvm::StringRef("TODO"));
  //   status |= fn(compiler, cgc);
  //   return status;

  // } else if (auto mod = msf.get<syn::ModuleDecl *>()) {
  //   stone::GenIR(cgc, *mod, compiler.GetInvocation().GetLangContext(),
  //                OutputFile("TODO"));
  //   status |= fn(compiler, cgc);
  //   return status;
  // }
  status.SetIsError();
  return status;
}
static CompileStatus GenModule(CompilerInstance &compiler,
                               CodeGenContext &cgc) {
  return CompileStatus::MakeSuccess();
}

static CompileStatus CompileWithGenNative(CompilerInstance &compiler,
                                          CodeGenContext &cgc) {

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
  stone::GenNative(cgc, compiler.GetSyntaxContext(), llvm::StringRef() /*TODO*/,
                   compiler.GetInvocation().GetListener());

  return CompileStatus::MakeSuccess();
}

CompileStatus CompilerInstance::CompileWithCodeGen() {

  assert(GetInvocation().GetCompilerOptions().GetMode().CanCodeGen() &&
         "Mode does not support code gen");

  auto *mainModuleDecl = GetModuleSystem().GetMainModule();

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

  switch (GetInvocation().GetCompilerOptions().GetMode().GetKind()) {
  case ModeKind::EmitModule:
    return CompileWithGenIR(
        *this, mainModuleDecl, cgc,
        [&](CompilerInstance &compiler, CodeGenContext &cgc) {
          return GenModule(compiler, cgc);
        });
  case ModeKind::EmitIR:
    return CompileWithGenIR(
        *this, mainModuleDecl, cgc,
        [&](CompilerInstance &compiler, CodeGenContext &cgc) {
          return DumpIR(compiler, cgc);
        });
  case ModeKind::PrintIR:
    return CompileWithGenIR(
        *this, mainModuleDecl, cgc,
        [&](CompilerInstance &compiler, CodeGenContext &cgc) {
          return PrintIR(compiler, cgc);
        });
  default:
    return CompileWithGenIR(
        *this, mainModuleDecl, cgc,
        [&](CompilerInstance &compiler, CodeGenContext &cgc) {
          return CompileWithGenNative(compiler, cgc);
        });
  }
}

static CompileStatus DumpSyntax(syn::SyntaxFile &sf) {
  return CompileStatus::MakeSuccess();
}

static CompileStatus PrintSyntax(CompilerInstance &compiler) {
  return CompileStatus::MakeSuccess();
}

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
    break;
  }
  case TypeCheckMode::EachFile: {
    for (auto *syntaxFile :
         GetModuleSystem().GetMainModule()->GetPrimarySyntaxFiles()) {
      client(*syntaxFile, invocation.GetTypeCheckerOptions(),
             invocation.GetListener());
    }
    break;
  }
  default: {
  }
  }
}
CompileStatus CompilerInstance::CompileWithParsing() {
  return CompileWithParsing(
      [&](syn::SyntaxFile &) { return CompileStatus::MakeSuccess(); });
}

CompileStatus
CompilerInstance::CompileWithParsing(ParsingCompletedCallback fn) {

  for (auto moduleFile : GetModuleSystem().GetMainModule()->GetFiles()) {
    if (auto *syntaxFile = llvm::dyn_cast<syn::SyntaxFile>(moduleFile)) {
      syn::Parse(*syntaxFile, GetSyntaxContext(), invocation.GetListener());
      fn(*syntaxFile);
    }
  }

  if (!invocation.GetCompilerOptions().GetMode().JustParse()) {
    ResolveImports();
  }
  if (invocation.GetListener()) {
    invocation.GetListener()->OnSyntaxAnalysisCompleted(*this);
  }
  return CompileStatus::MakeSuccess();
}

void CompilerInstance::ResolveImports() {
  // Resolve imports for all the source files.
  for (auto *moduleFile : GetModuleSystem().GetMainModule()->GetFiles()) {
    if (auto *syntaxFile = dyn_cast<SyntaxFile>(moduleFile))
      sem::ResolveImports(*syntaxFile);
  }
}
CompileStatus CompilerInstance::CompileWithTypeChecking() {
  return CompileWithTypeChecking(
      [&](CompilerInstance &) { return CompileStatus::MakeSuccess(); });
}

CompileStatus
CompilerInstance::CompileWithTypeChecking(TypeCheckingCompletedCallback fn) {

  CompileStatus status = CompileWithParsing();
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
  return fn(*this);
}

CompileStatus CompilerInstance::Compile() {

  assert(CanCompile() && "Unknown mode -- cannot continue with compile!");

  if (GetInvocation().GetListener()) {
    GetInvocation().GetListener()->OnCompileStarted(*this);
  }
  // TODO: Future CreateSyntax();

  CompileStatus status;
  switch (GetInvocation().GetCompilerOptions().GetMode().GetKind()) {
  case ModeKind::Parse:
    status |= CompileWithParsing();
    break;
  case ModeKind::DumpSyntax:
    status |=
        CompileWithParsing([&](syn::SyntaxFile &sf) { return DumpSyntax(sf); });
    break;
  case ModeKind::TypeCheck:
    status |= CompileWithTypeChecking();
    break;
  case ModeKind::PrintSyntax:
    status |= CompileWithTypeChecking(
        [&](CompilerInstance &compiler) { return PrintSyntax(*this); });
    break;
  default:
    status |= CompileWithTypeChecking(
        [&](CompilerInstance &compiler) { return CompileWithCodeGen(); });
    break;
  }
  // For now
  return status;
}
