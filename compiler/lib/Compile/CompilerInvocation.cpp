#include "stone/Compile/CompilerCommandLine.h"
#include "stone/Compile/CompilerOptionsConverter.h"
#include "stone/Diag/CompilerDiagnostic.h"

#include "llvm/Support/BuryPointer.h"
#include "llvm/Support/CrashRecoveryContext.h"
#include "llvm/Support/Errc.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/LockFileManager.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/Program.h"
#include "llvm/Support/Signals.h"
#include "llvm/Support/TimeProfiler.h"
#include "llvm/Support/Timer.h"
#include "llvm/Support/raw_ostream.h"

using namespace stone;

CompilerInvocation::CompilerInvocation() {
  llvm::sys::fs::current_path(GetCompilerOptions().workDirectory);
  SetTargetTriple(llvm::sys::getDefaultTargetTriple());
}

static Status ParseCompilerAction(llvm::opt::InputArgList &args,
                                  CompilerOptions &compilerOpts) {
  auto actionArg = args.getLastArg(opts::ModeGroup);
  if (actionArg) {
    auto actionKind = opts::GetActionKindByOptionID(opts::GetArgID(actionArg));
    compilerOpts.GetAction().SetKind(actionKind);
    if (compilerOpts.GetAction().IsAlien()) {
      GetDiags().PrintD(diag::err_alien_mode);
      return Status::Error();
    }
    compilerOpts.GetAction().SetName(opts::GetArgName(actionArg));
  } else {
    // We just default to emitting an object file since nothing was presented.
    compilerOpts.GetAction().SetKind(ActionKind::None);
  }
  return Status();
}
static Status ParseCompilerOptions(llvm::opt::InputArgList &args,
                                   LangOptions &langOpts,
                                   CompilerOptions &compilerOpts,
                                   ModuleOptions &moduleOptions,
                                   DiagnosticEngine &diags,
                                   MemoryBuffers *buffers) {
  CompilerOptionsConverter converter(args, diags, langOpts, compilerOpts,
                                     config.GetModuleOptions());
  return converter.Convert(buffers);
}

static Status ParseLangOptions(llvm::opt::InputArgList &args) {
  return Status();
}

static Status ParseTypeCheckerOptions(llvm::opt::InputArgList &args) {
  return Status();
}
static Status ParseSearchPathOptions(llvm::opt::InputArgList &args) {
  return Status();
}

static void ComputeCodeCodeGenOutputKind(const CompilerOptions &compilerOpts,
                                         CodeGenOptions &codeGenOpts) {

  // TODO: You are missing a few -- OK for now
  switch (compilerOpts.GetAction().GetKind()) {
  case ActionKind::EmitModule:
    codeGenOpts.codeGenOutputKind = CodeGenOutputKind::LLVMModule;
  case ActionKind::EmitIRBefore:
    codeGenOpts.codeGenOutputKind = CodeGenOutputKind::LLVMIRPreOptimization;
  case ActionKind::EmitIRAfter:
    codeGenOpts.codeGenOutputKind = CodeGenOutputKind::LLVMIRPostOptimization;
  case ActionKind::EmitBC:
    codeGenOpts.codeGenOutputKind = CodeGenOutputKind::LLVMBitCode;
    break;
  case ActionKind::EmitAssembly:
    codeGenOpts.codeGenOutputKind = CodeGenOutputKind::NativeAssembly;
    break;
  default:
    codeGenOpts.codeGenOutputKind = CodeGenOutputKind::ObjectFile;
    break;
  }
}

static Status ParseCodeGenOptions(llvm::opt::InputArgList &args) {

  ComputeCodeCodeGenOutputKind(GetCompilerOptions(), GetCodeGenOptions());

  return Status();
}
static Status ParseTargetOptions(llvm::opt::InputArgList &args) {

  // std::tie(GetCodeGenOptions().llvmTargetOpts, GetCodeGenOptions().targetCPU,
  //          GetCodeGenOptions().targetFeatures,
  //          GetCodeGenOptions().effectiveClangTriple) =
  //     stone::GetIRTargetOptions(GetCodeGenOptions(),
  //                               GetLangContext().GetLangOptions(),
  //                               *clangContext);

  // if (clangContext.GetInstance().getLangOpts().PointerAuthCalls) {
  //   SetPointerAuthOptions(const_cast<CodeGenOptions
  //   &>(GetCodeGenOptions).pointerAuth,
  // clangContext.GetInstance().getCodeGenOpts().PointerAuth);
  // }
  return Status();
}

// // TODO: Look at SetupWorkingDirectory
// static llvm::StringRef ParseWorkDirectory(const llvm::opt::InputArgList
// &args)
// {
//   if (auto *arg = args.getLastArg(opts::WorkDir)) {
//     llvm::SmallString<128> smallStr;
//     smallStr = arg->getValue();
//     llvm::sys::fs::make_absolute(smallStr);
//     return smallStr.str();
//   }
//   return llvm::StringRef();
// }

Status CompilerInvocation::ParseArgs(llvm::ArrayRef<const char *> args) {

  unsigned includedFlagsBitmask = 0;
  unsigned excludedFlagsBitmask;
  unsigned missingArgIndex;
  unsigned missingArgCount;

  auto compilerOptionTable = opts::CreateOptTable();
  auto compilerInputArgList =
      std::make_unique<llvm::opt::InputArgList>(compilerOptionTable->ParseArgs(
          args, missingArgIndex, missingArgCount, includedFlagsBitmask,
          excludedFlagsBitmask));

  assert(compilerInputArgList && "No input argument list.");

  if (missingArgCount) {
    GetDiags().PrintD(
        SrcLoc(), diag::err_missing_arg_value,
        diag::LLVMStr(compilerInputArgList->getArgString(missingArgIndex)),
        diag::UInt(missingArgCount));
    return Status::Error();
  }
  // Check for unknown arguments.
  for (const llvm::opt::Arg *arg :
       compilerInputArgList->filtered(opts::UNKNOWN)) {
    GetDiags().PrintD(SrcLoc(), diag::err_unknown_arg,
                      diag::LLVMStr(arg->getAsString(*compilerInputArgList)));
  }
  // Ok for now.
  // if (config.GetDiags().HasError()) {
  //   return nullptr;
  // }
  if (ParseCompilerAction(*compilerInputArgList).IsError()) {
    return Status::Error();
  }

  if (ParseCompilerOptions(*compilerInputArgList,
                           nullptr /* pass null for now*/)
          .IsError()) {

    return Status::Error();
  }

  // if (ParseTypeCheckerOptions(*compilerInputArgList).IsError()) {
  //   return Status::Error();
  // }

  // if (ParseSearchPathOptions(*compilerInputArgList).IsError()) {

  //   return Status::Error();
  // }

  // if (ParseCodeGenOptions(*compilerInputArgList).IsError()) {
  //   return Status::Error();
  // }

  // if (ParseTargetOptions(*compilerInputArgList).IsError()) {
  //   return Status::Error();
  // }

  // CreateSourceBuffers();

  return Status();
}

void CompilerInvocation::SetTargetTriple(llvm::StringRef triple) {}
void CompilerInvocation::SetTargetTriple(const llvm::Triple &Triple) {}

void CompilerInvocation::SetMainExecutable(const char *arg0, void *mainAddr) {
  // compilerOpts.MainExecutablePath =
  //     llvm::sys::fs::getMainExecutable(arg0, mainAddr);
  // compilerOpts.MainExecutableName =
  //     file::GetStem(GetCompilerOptions().MainExecutablePath);
}
