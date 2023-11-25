#include "stone/Basic/Defer.h"
#include "stone/Basic/LLVMInit.h"
#include "stone/Basic/MainExecutablePath.h"
#include "stone/CodeCompletionListener.h"
#include "stone/Compile/CompilerInstance.h"
#include "stone/Compile/CompilerInvocation.h"
#include "stone/Diag/CompilerDiagnostic.h"
#include "stone/Diag/TextDiagnosticFormatter.h"
#include "stone/Diag/TextDiagnosticListener.h"
#include "stone/Gen/CodeGenContext.h"
#include "stone/Public.h"
#include "stone/Session/ModeKind.h"
#include "stone/Syntax/Module.h"
#include "stone/Syntax/SyntaxDiagnosticArgument.h"

#include "clang/Basic/TargetInfo.h"

#include "llvm/IR/Module.h"
#include "llvm/Support/Casting.h"

using namespace stone;
using namespace stone::syn;

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
    invocation.GetLangContext().GetDiags().PrintD(SrcLoc(),
                                                  diag::err_no_input_files);
    return Finish(Error(true));
  }
  // We setup clang now -- this just loads the instance.
  if (invocation.SetupClang("-cc1", arg0).HasError()) {

    return Finish(Error(true));
  }

  // Setup the custom formatting to be able to handle syntax diagnostics
  SyntaxDiagnosticFormatter diagFormatter;
  SyntaxDiagnosticEmitter diagEmitter(diagFormatter);
  TextDiagnosticListener diagListener(diagEmitter);

  invocation.GetLangContext().GetDiags().AddListener(diagListener);

  ConfigurationFileBuffers configurationFileBuffers;

  // Parse arguments.
  auto ial = invocation.ParseArgs(args);
  if (!ial) {
    return Finish(Error(true));
  }
  if (invocation.HasError()) {
    return Finish(Error(true));
  }
  if (invocation.ComputeOptions(*ial).HasError()) {
    return Finish(Error(true));
  }
  if (invocation.GetMainMode().IsAlien()) {
    invocation.GetLangContext().GetDiags().PrintD(SrcLoc(),
                                                  diag::err_alien_mode);
    Finish(Error(true));
  }
  if (invocation.GetMainMode().IsPrintHelp()) {
    // TODO: invocation.PrintHelp(invocation.GetOpts());
    return Finish();
  }
  if (invocation.GetMainMode().IsPrintVersion()) {
    invocation.PrintVersion();
    return Finish(Error(true));
  }
  if (!invocation.GetMainMode().CanCompile()) {
    /// invocation.PrintD()
    return Finish();
  }
  if (invocation.GetListener()) {
    invocation.GetListener()->OnCompileConfigured(invocation);
  }
  if (invocation.CreateSourceBuffers().HasError()) {
    return Finish();
  }

  CompilerInstance compiler(invocation);
  auto status = compiler.Compile();

  if (status.IsError() || invocation.HasError()) {
    return Finish(Error(true));
  }
  return Finish();
}

static Status DumpIR(CompilerInstance &compiler, CodeGenContext &cgc) {
  Status::Success();
}

static Status PrintIR(CompilerInstance &compiler, CodeGenContext &cgc) {
  Status::Success();
}

Status CompilerInstance::CompileWithGenIR(CodeGenContext &cgc,
                                          IRCodeGenCompletedCallback notifiy) {
  const auto &invocation = GetInvocation();
  const CompilerOptions &compilerOpts = invocation.GetCompilerOptions();

  if (IsWholeModuleCodeGen()) {
    auto *mainModule = GetModuleSystem().GetMainModule();
    const PrimaryFileSpecificPaths primaryFileSpecificPaths =
        GetPrimaryFileSpecificPathsForWholeModuleOptimizationMode();

    stone::GenModuleIR(cgc, primaryFileSpecificPaths.outputFilename, mainModule,
                       primaryFileSpecificPaths);
  } else if (IsSyntaxFileCodeGen()) {
    for (auto *primarySyntaxFile : GetPrimarySyntaxFiles()) {
      const PrimaryFileSpecificPaths primaryFileSpecificPaths =
          GetPrimaryFileSpecificPathsForSyntaxFile(*primarySyntaxFile);
      stone::GenSyntaxFileIR(cgc, primaryFileSpecificPaths.outputFilename,
                             primarySyntaxFile, primaryFileSpecificPaths);
    }
  }

  if (notifiy) {
    notifiy(*this, cgc);
  }
  Status::Error();
}
static Status GenModule(CompilerInstance &compiler, CodeGenContext &cgc) {
  return Status::Success();
}

Status CompilerInstance::CompileWithGenNative(CodeGenContext &cgc) {

  auto result = stone::GenNative(cgc, GetSyntaxContext(), llvm::StringRef(),
                                 GetInvocation().GetListener());
  return Status::Success();
}

Status CompilerInstance::CompileWithCodeGen() {

  assert(CanCodeGen() && "Mode does not support code gen");

  // We are performing some low level code generation
  CodeGenContext cgc(
      GetInvocation().GetCodeGenOptions(), GetInvocation().GetModuleOptions(),
      GetInvocation().GetTargetOptions(), GetInvocation().GetLangContext(),
      GetInvocation().GetClangContext());

  // auto *Module = IGM.getModule();
  // assert(Module && "Expected llvm:Module for IR generation!");

  // Module->setTargetTriple(IGM.Triple.str());

  // // Set the module's string representation.
  // Module->setDataLayout(IGM.DataLayout.getStringRepresentation());

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

  switch (GetInvocation().GetCodeGenOptions().codeGenOutputKind) {
  case CodeGenOutputKind::LLVMModule:
    return CompileWithGenIR(
        cgc, [&](CompilerInstance &compiler, CodeGenContext &cgc) {
          return GenModule(*this, cgc);
        });
  case CodeGenOutputKind::LLVMIRPreOptimization:
  case CodeGenOutputKind::LLVMIRPostOptimization:
    return CompileWithGenIR(
        cgc, [&](CompilerInstance &compiler, CodeGenContext &cgc) {
          return DumpIR(*this, cgc);
        });
  // case CodeGenOutputKind::PrintIR:
  //   return CompileWithGenIR(
  //       cgc, [&](CompilerInstance &compiler, CodeGenContext &cgc) {
  //         return PrintIR(*this, cgc);
  //       });
  default:
    return CompileWithGenIR(
        cgc, [&](CompilerInstance &compiler, CodeGenContext &cgc) {
          return CompileWithGenNative(cgc);
        });
  }
}

static Status DumpSyntax(CompilerInstance &compiler, syn::SyntaxFile &sf) {
  return Status::Success();
}

static Status PrintSyntax(CompilerInstance &compiler) {
  return Status::Success();
}

Status CompilerInstance::CompileWithParsing() {
  return CompileWithParsing(
      [&](syn::SyntaxFile &) { return Status::Success(); });
}

Status CompilerInstance::CompileWithParsing(ParsingCompletedCallback notifiy) {

  for (auto moduleFile : GetModuleSystem().GetMainModule()->GetFiles()) {
    if (auto *syntaxFile = llvm::dyn_cast<syn::SyntaxFile>(moduleFile)) {
      stone::ParseSyntaxFile(*syntaxFile, GetSyntaxContext(),
                             invocation.GetListener());
      if (notifiy) {
        notifiy(*syntaxFile);
      }
    }
  }

  if (!invocation.GetMainMode().JustParse()) {
    ResolveImports();
  }
  if (invocation.GetListener()) {
    invocation.GetListener()->OnSyntaxAnalysisCompleted(*this);
  }
  return Status::Success();
}

Status CompilerInstance::CompileWithTypeChecking() {
  return CompileWithTypeChecking(
      [&](CompilerInstance &) { return Status::Success(); });
}

Status CompilerInstance::CompileWithTypeChecking(
    TypeCheckingCompletedCallback notifiy) {

  auto status = CompileWithParsing();
  if (status.IsError()) {
    return status;
  }
  ForEachSyntaxFile([&](SyntaxFile &syntaxFile,
                        TypeCheckerOptions &typeCheckerOpts,
                        stone::TypeCheckerListener *listener) {
    stone::TypeCheckSyntaxFile(syntaxFile, typeCheckerOpts, listener);
  });

  // TODO: FinishTypeCheck();
  if (invocation.GetListener()) {
    invocation.GetListener()->OnSemanticAnalysisCompleted(*this);
  }
  return notifiy(*this);
}
Status CompilerInstance::Compile() {

  assert(CanCompile() && "Unknown mode -- cannot continue with compile!");
  llvm::TimeTraceScope compileTimeScope("Compile");

  if (GetInvocation().GetListener()) {
    GetInvocation().GetListener()->OnCompileStarted(*this);
  }
  if (IsCompileForLLVMIR()) {
    return CompileForLLVMIR();
  }
  return CompileForParse(invocation.GetMainMode().GetKind());
}

Status CompilerInstance::CompileForParse(ModeKind kind) {

  if (CompileForParse([&](SyntaxFile &syntaxFile) {
        switch (kind) {
        case ModeKind::Parse:
          return Status();
        case ModeKind::ResolveImports:
          return CompileForResolveImports(syntaxFile);
        case ModeKind::DumpSyntax:
          return CompileForDumpSyntax(syntaxFile);
        default:
          return Status();
        }
      }).IsErrorOrHasCompletion()) {
      return Status::CompletionAndIsError();
  }
  return CompileForTypeCheck(kind);
}

Status CompilerInstance::CompileForParse(
    std::function<Status(syn::SyntaxFile &syntaxFile)> notify) {
  for (auto moduleFile : GetModuleSystem().GetMainModule()->GetFiles()) {
    if (auto *syntaxFile = llvm::dyn_cast<syn::SyntaxFile>(moduleFile)) {
      stone::ParseSyntaxFile(*syntaxFile, GetSyntaxContext(),
                             invocation.GetListener());
      // If there are no errors
      if (notify(*syntaxFile).IsError()) {
        return Status::Error();
      }
    }
  }

  return Status();
}

Status CompilerInstance::CompileForDumpSyntax(syn::SyntaxFile &syntaxFile) {}

Status CompilerInstance::CompileForResolveImports(syn::SyntaxFile &syntaxFile) {
}

Status CompilerInstance::CompileForTypeCheck(ModeKind kind) {
  Status status;
  if (CompileForTypeCheck([&](SyntaxFile &syntaxFile) {
        switch (kind) {
        case ModeKind::TypeCheck:
          return Status();
        case ModeKind::PrintSyntax:
          return CompileForPrintSyntax(syntaxFile);
        default:
          return Status();
        }
      }).IsErrorOrHasCompletion()) {
    status.SetHasCompletionAndIsError();
    return status;
  }
  return CompileForGenerateIR(kind);
}

Status CompilerInstance::CompileForTypeCheck(
    std::function<Status(syn::SyntaxFile &)> notifiy) {
  return Status();
}

Status CompilerInstance::CompileForPrintSyntax(syn::SyntaxFile &syntaxFile) {}

Status CompilerInstance::CompileForGenerateIR(ModeKind kind) {
  Status status;
  CodeGenContext codeGenContext(
      GetInvocation().GetCodeGenOptions(), GetInvocation().GetModuleOptions(),
      GetInvocation().GetTargetOptions(), GetInvocation().GetLangContext(),
      GetInvocation().GetClangContext());

  if (CompileForGenerateIR([&](CodeGenContext &codeGenContext) {
        switch (kind) {
        case ModeKind::EmitModule:
          return Status();
        case ModeKind::EmitIRPre:
          return Status(); // Print it out
        case ModeKind::EmitIR:
          return Status(); // Print it out
        case ModeKind::MergeModules:
          return Status();
        default: {
          return Status();
        }
        }
      }).IsErrorOrHasCompletion()) {
    status.SetHasCompletionAndIsError();
    return status;
  }
  return CompileForEmitNative(kind, codeGenContext);
}

Status CompilerInstance::CompileForGenerateIR(
    std::function<Status(CodeGenContext &)> notifiy) {}

Status CompilerInstance::CompileForEmitNative(ModeKind kind,
                                              CodeGenContext &codeGenContext) {
  // switch (kind) {
  // case ModeKind::EmitBC:
  // case ModeKind::EmitObject:
  // case ModeKind::EmitLibrary:
  // case ModeKind::EmitModule:
  // case ModeKind::EmitAssembly:
  //   return CompileForEmitNative(codeGenContext);
  // default: {
  //   llvm_unreachable("Invalid mode!");
  // }
  // }

  // TryFreeASTContext();

  return Status();
}
