#include "stone/Compile/CompilerContextBuilder.h"
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

CompilerContext::CompilerContext() {
  llvm::sys::fs::current_path(GetCompilerOptions().workDirectory);
  SetTargetTriple(llvm::sys::getDefaultTargetTriple());
}

void CompilerContext::SetTargetTriple(llvm::StringRef triple) {}
void CompilerContext::SetTargetTriple(const llvm::Triple &Triple) {}
void CompilerContext::SetMainExecutable(const char *arg0, void *mainAddr) {}

CompilerContextBuilder::CompilerContextBuilder() {}

std::unique_ptr<CompilerContext>
CompilerContextBuilder::Build(llvm::ArrayRef<const char *> args,
                              DiagnosticEngine &diags) {

  unsigned includedFlagsBitmask = 0;
  unsigned excludedFlagsBitmask;
  unsigned missingArgIndex;
  unsigned missingArgCount;

  auto compilerContext = std::make_unique<CompilerContext>();
  auto compilerOptionTable = opts::CreateOptTable();
  auto compilerInputArgList =
      std::make_unique<llvm::opt::InputArgList>(compilerOptionTable->ParseArgs(
          args, missingArgIndex, missingArgCount, includedFlagsBitmask,
          excludedFlagsBitmask));

  assert(compilerInputArgList && "No input argument list.");

  if (missingArgCount) {
    diags.PrintD(
        SrcLoc(), diag::err_missing_arg_value,
        diag::LLVMStr(compilerInputArgList->getArgString(missingArgIndex)),
        diag::UInt(missingArgCount));
    return nullptr;
  }
  // Check for unknown arguments.
  for (const llvm::opt::Arg *arg :
       compilerInputArgList->filtered(opts::UNKNOWN)) {
    diags.PrintD(SrcLoc(), diag::err_unknown_arg,
                 diag::LLVMStr(arg->getAsString(*compilerInputArgList)));
  }
  // Ok for now.
  // if (diags.HasError()) {
  //   return nullptr;
  // }
  // if (ParseCompilerAction(*compilerInputArgList).IsError()) {
  //   return Status::Error();
  // }

  // if (ParseCompilerOptions(*compilerInputArgList,
  //                          nullptr /* pass null for now*/)
  //         .IsError()) {

  //   return Status::Error();
  // }

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

  return compilerContext;
}

// Status CompilerContextBuilder::ParseCompilerAction(llvm::opt::InputArgList
// &args) {
//   auto actionArg = args.getLastArg(opts::ModeGroup);
//   if (actionArg) {
//     auto actionKind =
//     opts::GetActionKindByOptionID(opts::GetArgID(actionArg));
//     GetAction().SetKind(actionKind);
//     if (GetAction().IsAlien()) {
//       return Status::Error();
//     }
//     GetAction().SetName(opts::GetArgName(actionArg));
//   }
//   return Status();
// }

// Status CompilerContextBuilder::ParseCompilerOptions(llvm::opt::InputArgList
// &args,
//                                                  MemoryBuffers *buffers) {

//   CompilerOptionsConverter converter(GetLangContext().GetDiags(), args,
//                                      GetLangContext().GetLangOptions(),
//                                      GetCompilerOptions(),
//                                      GetModuleOptions());
//   return converter.Convert(buffers);
// }
// Status CompilerContextBuilder::ParseLangOptions(llvm::opt::InputArgList
// &args) {
//   return Status();
// }

// Status
// CompilerContextBuilder::ParseTypeCheckerOptions(llvm::opt::InputArgList
// &args) {
//   return Status();
// }
// Status
// CompilerContextBuilder::ParseSearchPathOptions(llvm::opt::InputArgList &args)
// {
//   return Status();
// }

// static void ComputeCodeCodeGenOutputKind(const CompilerOptions &compilerOpts,
//                                          CodeGenOptions &codeGenOpts) {

//   // TODO: You are missing a few -- OK for now
//   switch (compilerOpts.GetAction().GetKind()) {
//   case ActionKind::EmitModule:
//     codeGenOpts.codeGenOutputKind = CodeGenOutputKind::LLVMModule;
//   case ActionKind::EmitIRBefore:
//     codeGenOpts.codeGenOutputKind = CodeGenOutputKind::LLVMIRPreOptimization;
//   case ActionKind::EmitIRAfter:
//     codeGenOpts.codeGenOutputKind =
//     CodeGenOutputKind::LLVMIRPostOptimization;
//   case ActionKind::EmitBC:
//     codeGenOpts.codeGenOutputKind = CodeGenOutputKind::LLVMBitCode;
//     break;
//   case ActionKind::EmitAssembly:
//     codeGenOpts.codeGenOutputKind = CodeGenOutputKind::NativeAssembly;
//     break;
//   default:
//     codeGenOpts.codeGenOutputKind = CodeGenOutputKind::ObjectFile;
//     break;
//   }
// }
// Status CompilerContextBuilder::ParseCodeGenOptions(llvm::opt::InputArgList
// &args) {

//   ComputeCodeCodeGenOutputKind(GetCompilerOptions(), GetCodeGenOptions());

//   return Status();
// }
// Status CompilerContextBuilder::ParseTargetOptions(llvm::opt::InputArgList
// &args)
// {

//   std::tie(GetCodeGenOptions().llvmTargetOpts, GetCodeGenOptions().targetCPU,
//            GetCodeGenOptions().targetFeatures,
//            GetCodeGenOptions().effectiveClangTriple) =
//       stone::GetIRTargetOptions(GetCodeGenOptions(),
//                                 GetLangContext().GetLangOptions(),
//                                 *clangContext);

//   // if (clangContext.GetInstance().getLangOpts().PointerAuthCalls) {
//   //   SetPointerAuthOptions(const_cast<CodeGenOptions
//   //   &>(GetCodeGenOptions).pointerAuth,
//   // clangContext.GetInstance().getCodeGenOpts().PointerAuth);
//   // }
//   return Status();
// }

// // TODO: Look at SetupWorkingDirectory
// llvm::StringRef

// CompilerContextBuilder::ParseWorkDirectory(const llvm::opt::InputArgList
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
