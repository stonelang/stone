#include "stone/Compile/CompilerInvocation.h"
#include "stone/AST/DiagnosticsCompile.h"
#include "stone/Compile/Compile.h"
#include "stone/Compile/CompilerInstance.h"
#include "stone/Compile/CompilerOptionsConverter.h"
#include "stone/Compile/CompilerFrontend.h"
#include "stone/Parse/CodeCompletionCallbacks.h"

#include "stone/Strings.h"
#include "stone/Support/Options.h"

#include "llvm/Support/BuryPointer.h"
#include "llvm/Support/CrashRecoveryContext.h"
#include "llvm/Support/Errc.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/LockFileManager.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/Program.h"
#include "llvm/Support/Signals.h"
#include "llvm/Support/TimeProfiler.h"
#include "llvm/Support/Timer.h"
#include "llvm/Support/raw_ostream.h"

using namespace stone;

CompilerInvocation::CompilerInvocation() : clangImporter(new ClangImporter()) {}

void CompilerInvocation::SetTargetTriple(llvm::StringRef triple) {
  langOpts.SetTarget(llvm::Triple(triple));
}

static Status ParseCompilerOptions(llvm::opt::InputArgList &args,
                                   LangOptions &langOpts,
                                   CompilerOptions &compilerOpts,
                                   DiagnosticEngine &diags,
                                   MemoryBuffers *buffers) {

  CompilerOptionsConverter converter(args, diags, langOpts, compilerOpts);
  return converter.Convert(buffers);
}

static Status ParseTypeCheckerOptions(llvm::opt::InputArgList &ial,
                                      CompilerOptions &compilerOpts,
                                      TypeCheckerOptions &typeCheckerOpts,
                                      DiagnosticEngine &de) {
  return Status();
}

static std::optional<llvm::CodeModel::Model>
GetCodeModel(const CodeGenOptions &codeGenOpts) {

  unsigned codeModel = llvm::StringSwitch<unsigned>(codeGenOpts.codeModel)
                           .Case("tiny", llvm::CodeModel::Tiny)
                           .Case("small", llvm::CodeModel::Small)
                           .Case("kernel", llvm::CodeModel::Kernel)
                           .Case("medium", llvm::CodeModel::Medium)
                           .Case("large", llvm::CodeModel::Large)
                           .Case("default", ~1u)
                           .Default(~0u);
  assert(codeModel != ~0u && "invalid code model!");
  if (codeModel == ~1u) {
    return std::nullopt;
  }
  return static_cast<llvm::CodeModel::Model>(codeModel);
}

// TODO: cleanup
// static llvm::CodeGenOpt::Level
// GetOptimizationLevel(const CodeGenOptions &codeGenOpts) {
//   switch (codeGenOpts.optimizationLevel) {
//   default:
//     llvm_unreachable("Invalid optimization level!");
//   case OptimizationLevel::None:
//     return llvm::CodeGenOpt::None;
//   case OptimizationLevel::Less:
//     return llvm::CodeGenOpt::Less;
//   case OptimizationLevel::Default:
//     return llvm::CodeGenOpt::Default;
//   case OptimizationLevel::Aggressive:
//     return llvm::CodeGenOpt::Aggressive;
//   }
// }

IRTargetOptions stone::GetIRTargetOptions(const CodeGenOptions &codeGenOpts,
                                          const LangOptions &langOpts,
                                          ClangImporter &clangImporter) {
  llvm::TargetOptions llvmTargetOpts;
  // Explicitly request debugger tuning for LLDB which is the default
  // on Darwin platforms but not on others.
  llvmTargetOpts.DebuggerTuning = llvm::DebuggerKind::LLDB;

  llvmTargetOpts.FunctionSections = codeGenOpts.functionSections;

  switch (langOpts.threadModelKind) {
  case LangOptions::ThreadModelKind::POSIX:
    llvmTargetOpts.ThreadModel = llvm::ThreadModel::POSIX;
    break;
  case LangOptions::ThreadModelKind::Single:
    llvmTargetOpts.ThreadModel = llvm::ThreadModel::Single;
    break;
  }
  clang::TargetOptions &clangTargetOpts =
      clangImporter.GetClangInstance().getTarget().getTargetOpts();

  return std::make_tuple(llvmTargetOpts, clangTargetOpts.CPU,
                         clangTargetOpts.Features, clangTargetOpts.Triple);
}
static Status
ParseTargetOptions(llvm::opt::InputArgList &ial, CompilerOptions &compilerOpts,
                   CodeGenOptions &codeGenOpts, LangOptions &langOpts,
                   ClangImporter &clangImporter, DiagnosticEngine &de) {

  // tie the values to CodeGenOptions
  std::tie(codeGenOpts.llvmTargetOpts, codeGenOpts.targetCPU,
           codeGenOpts.targetFeatures, codeGenOpts.effectiveClangTriple) =
      stone::GetIRTargetOptions(codeGenOpts, langOpts, clangImporter);

  //   if (clangImporter.GetClangInstance().getLangOpts().PointerAuthCalls) {
  //     SetPointerAuthOptions(const_cast<CodeGenOptions
  //     &>(codeGenOpts).pointerAuth,
  //                            cc.GetInstance().getCodeGenOpts().PointerAuth);
  //   }
  return Status();
}

static Status ParseCodeGenOptions(llvm::opt::InputArgList &ial,
                                  CompilerInvocation &invocation,
                                  DiagnosticEngine &de,
                                  CompilerOptions &compilerOpts,
                                  CodeGenOptions &codeGenOpts) {

  switch (invocation.GetCompilerOptions().GetPrimaryAction()) {
  case CompilerActionKind::EmitModule:
    codeGenOpts.codeGenOutputKind = CodeGenOutputKind::LLVMModule;
  case CompilerActionKind::EmitIR: {
    if (ial.hasArg(opts::OPT_NoIROptimization)) {
      codeGenOpts.noIROptimization = true;
      codeGenOpts.codeGenOutputKind = CodeGenOutputKind::LLVMIRPreOptimization;
    } else {
      codeGenOpts.codeGenOutputKind = CodeGenOutputKind::LLVMIRPostOptimization;
    }
  }
  case CompilerActionKind::EmitBC:
    codeGenOpts.codeGenOutputKind = CodeGenOutputKind::LLVMBitCode;
    break;
  case CompilerActionKind::EmitAssembly:
    codeGenOpts.codeGenOutputKind = CodeGenOutputKind::NativeAssembly;
    break;
  default:
    codeGenOpts.codeGenOutputKind = CodeGenOutputKind::ObjectFile;
    break;
  }
  return Status();
}

Status CompilerInvocation::ParseArgs(llvm::ArrayRef<const char *> args) {

  inputArgList =
      std::make_unique<llvm::opt::InputArgList>(GetOptTable().ParseArgs(
          args, compilerOpts.missingArgIndex, compilerOpts.missingArgCount,
          compilerOpts.includedFlagsBitmask,
          compilerOpts.excludedFlagsBitmask));

  assert(inputArgList && "No input argument list.");

  if (compilerOpts.missingArgCount) {
    GetDiags().diagnose(
        SrcLoc(), diag::error_missing_arg_value,
        inputArgList->getArgString(compilerOpts.missingArgIndex),
        compilerOpts.missingArgCount);
    return Status::Error();
  }
  // Check for unknown arguments.
  for (const llvm::opt::Arg *arg : inputArgList->filtered(opts::OPT_UNKNOWN)) {
    GetDiags().diagnose(SrcLoc(), diag::error_unknown_arg,
                        arg->getAsString(*inputArgList));

    // TODO: Good for now. But, you want to print out all and check for diag
    // errors
    return Status::Error();
  }
  if (GetDiags().hadAnyError()) {
    return Status::Error();
  }
  // TODO: Pass MemoryBuffers in ParseCommandLine
  auto status = ParseCompilerOptions(*inputArgList, langOpts, compilerOpts,
                                     GetDiags(), nullptr);
  if (status.IsErrorOrHasCompletion()) {
    return status;
  }

  if (GetCompilerOptions().DoesActionGenerateIR() ||
      GetCompilerOptions().DoesActionGenerateNativeCode()) {
    // TODO: hard coding -cc1 for now -- build out proper string.
    if (SetupClang(strings::ClangCC1,
                   GetCompilerOptions().mainExecutablePath.data())
            .IsError()) {
      return Status::Error();
    }
    if (ParseTargetOptions(*inputArgList, compilerOpts, codeGenOpts, langOpts,
                           GetClangImporter(), GetDiags())
            .IsError()) {
      return Status::Error();
    }
  }
  if (ParseCodeGenOptions(*inputArgList, *this, GetDiags(),
                          GetCompilerOptions(), GetCodeGenOptions())
          .IsError()) {
    return Status::Error();
  }
  return Status();
}

const PrimaryFileSpecificPaths &
CompilerInvocation::GetPrimaryFileSpecificPathsForWholeModuleOptimizationMode()
    const {
  return GetPrimaryFileSpecificPathsForAtMostOnePrimary();
}
const PrimaryFileSpecificPaths &
CompilerInvocation::GetPrimaryFileSpecificPathsForAtMostOnePrimary() const {
  return GetCompilerOptions()
      .GetInputsAndOutputs()
      .GetPrimaryFileSpecificPathsForAtMostOnePrimary();
}
const PrimaryFileSpecificPaths &
CompilerInvocation::GetPrimaryFileSpecificPathsForPrimary(
    StringRef filename) const {
  return GetCompilerOptions()
      .GetInputsAndOutputs()
      .GetPrimaryFileSpecificPathsForPrimary(filename);
}
const PrimaryFileSpecificPaths &
CompilerInvocation::GetPrimaryFileSpecificPathsForSyntaxFile(
    const SourceFile &sf) const {
  return GetCompilerOptions()
      .GetInputsAndOutputs()
      .GetPrimaryFileSpecificPathsForPrimary(sf.GetFilename());
}

Status CompilerInvocation::SetupClang(llvm::ArrayRef<const char *> argv,
                                      const char *arg0) {
  return GetClangImporter().Setup(argv, arg0);
}