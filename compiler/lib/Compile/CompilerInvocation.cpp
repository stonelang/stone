#include "stone/Compile/CompilerInvocation.h"
#include "stone/Compile/Compiler.h"
#include "stone/Compile/CompilerOptionsConverter.h"
#include "stone/Core.h"
#include "stone/Strings.h"
#include "stone/Support/DiagnosticsCompile.h"
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

CompilerInvocation::CompilerInvocation()
    : clangContext(new ClangContext()), fileMgr(GetFileSystemOptions()) {

  llvm::sys::fs::current_path(GetCompilerOptions().workingDirectory);
  SetTargetTriple(llvm::sys::getDefaultTargetTriple());
}

void CompilerInvocation::SetTargetTriple(llvm::StringRef triple) {
  langOpts.SetTarget(llvm::Triple(triple));
}

/// \return the action from the ArgList
static CompilerAction CreateAction(const llvm::opt::ArgList &args) {
  return CompilerAction::None;
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
                                          ClangContext &clangContext) {
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
  // Set float ABI type.
  // assert((CodeGenOpts.FloatABI == "soft" || CodeGenOpts.FloatABI == "softfp"
  // ||
  //         CodeGenOpts.FloatABI == "hard" || CodeGenOpts.FloatABI.empty()) &&
  //        "Invalid Floating Point ABI!");
  // Options.FloatABIType =
  //     llvm::StringSwitch<llvm::FloatABI::ABIType>(CodeGenOpts.FloatABI)
  //         .Case("soft", llvm::FloatABI::Soft)
  //         .Case("softfp", llvm::FloatABI::Soft)
  //         .Case("hard", llvm::FloatABI::Hard)
  //         .Default(llvm::FloatABI::Default);

  clang::TargetOptions &clangTargetOpts =
      clangContext.GetInstance().getTarget().getTargetOpts();
  return std::make_tuple(llvmTargetOpts, clangTargetOpts.CPU,
                         clangTargetOpts.Features, clangTargetOpts.Triple);
}
static Status
ParseTargetOptions(llvm::opt::InputArgList &ial, CompilerOptions &compilerOpts,
                   CodeGenOptions &codeGenOpts, LangOptions &langOpts,
                   ClangContext &clangContext, DiagnosticEngine &de) {

  // tie the values to CodeGenOptions
  std::tie(codeGenOpts.llvmTargetOpts, codeGenOpts.targetCPU,
           codeGenOpts.targetFeatures, codeGenOpts.effectiveClangTriple) =
      stone::GetIRTargetOptions(codeGenOpts, langOpts, clangContext);

  //   if (clangContext.GetInstance().getLangOpts().PointerAuthCalls) {
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

  switch (invocation.GetCompilerOptions().GetMainAction()) {
  case CompilerAction::EmitModule:
    codeGenOpts.codeGenOutputKind = CodeGenOutputKind::LLVMModule;
  case CompilerAction::EmitIRBefore:
    codeGenOpts.codeGenOutputKind = CodeGenOutputKind::LLVMIRPreOptimization;
  case CompilerAction::EmitIRAfter:
    codeGenOpts.codeGenOutputKind = CodeGenOutputKind::LLVMIRPostOptimization;
  case CompilerAction::EmitBC:
    codeGenOpts.codeGenOutputKind = CodeGenOutputKind::LLVMBitCode;
    break;
  case CompilerAction::EmitAssembly:
    codeGenOpts.codeGenOutputKind = CodeGenOutputKind::NativeAssembly;
    break;
  default:
    codeGenOpts.codeGenOutputKind = CodeGenOutputKind::ObjectFile;
    break;
  }
  return Status();
}

Status CompilerInvocation::ParseCommandLine(llvm::ArrayRef<const char *> args) {

  unsigned includedFlagsBitmask = 0;
  unsigned excludedFlagsBitmask = opts::ExcludeCompilerOption;
  unsigned missingArgIndex;
  unsigned missingArgCount;

  inputArgList = std::make_unique<llvm::opt::InputArgList>(
      GetOptTable().ParseArgs(args, missingArgIndex, missingArgCount,
                              includedFlagsBitmask, excludedFlagsBitmask));

  assert(inputArgList && "No input argument list.");

  if (missingArgCount) {
    GetDiags().PrintD(
        SrcLoc(), diag::err_missing_arg_value,
        StringRef(inputArgList->getArgString(missingArgIndex)),
        (unsigned)missingArgCount);
    return Status::Error();
  }
  // Check for unknown arguments.
  for (const llvm::opt::Arg *arg : inputArgList->filtered(opts::OPT_UNKNOWN)) {
    GetDiags().PrintD(SrcLoc(), diag::err_unknown_arg,
                      StringRef(arg->getAsString(*inputArgList)));

    // TODO: Good for now. But, you want to print out all and check for diag
    // errors
    return Status::Error();
  }
  if (GetDiags().HasError()) {
    return Status::Error();
  }
  // TODO: Pass MemoryBuffers in ParseCommandLine
  if (ParseCompilerOptions(*inputArgList, langOpts, compilerOpts, GetDiags(),
                           nullptr)
          .IsError()) {
    return Status::Error();
  }
  if (GetCompilerOptions().DoesActionGenerateIR() ||
      GetCompilerOptions().DoesActionGenerateNative()) {
    // TODO: hard coding -cc1 for now -- build out proper string.
    if (SetupClang(strings::ClangCC1,
                   GetCompilerOptions().mainExecutablePath.data())
            .IsError()) {
      return Status::Error();
    }
    if (ParseTargetOptions(*inputArgList, compilerOpts, codeGenOpts, langOpts,
                           GetClangContext(), GetDiags())
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
  return clangContext->Setup(argv, arg0);
}