#include "stone/Compile/CompilerInvocation.h"
#include "stone/Compile/Compiler.h"
#include "stone/Compile/CompilerOptionsConverter.h"
#include "stone/Diag/CompilerDiagnostic.h"
#include "stone/Option/Options.h"

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

CompilerInvocation::CompilerInvocation(Compiler &compiler)
    : compiler(compiler), clangContext(new ClangContext()) {
  llvm::sys::fs::current_path(GetCompilerOptions().workingDirectory);
  SetTargetTriple(llvm::sys::getDefaultTargetTriple());
}

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

Status CompilerInvocation::ParseCommandLine(llvm::ArrayRef<const char *> args) {

  unsigned includedFlagsBitmask = 0;
  unsigned excludedFlagsBitmask = opts::NoCompilerOption;
  unsigned missingArgIndex;
  unsigned missingArgCount;

  auto compilerOptTable = stone::CreateOptTable();
  auto compilerInputArgList = std::make_unique<llvm::opt::InputArgList>(
      compilerOptTable->ParseArgs(args, missingArgIndex, missingArgCount,
                                  includedFlagsBitmask, excludedFlagsBitmask));

  assert(compilerInputArgList && "No input argument list.");

  if (missingArgCount) {
    compiler.GetDiags().PrintD(
        SrcLoc(), diag::err_missing_arg_value,
        diag::LLVMStr(compilerInputArgList->getArgString(missingArgIndex)),
        diag::UInt(missingArgCount));
    return Status::Error();
  }
  // Check for unknown arguments.
  for (const llvm::opt::Arg *arg :
       compilerInputArgList->filtered(opts::UNKNOWN)) {
    compiler.GetDiags().PrintD(
        SrcLoc(), diag::err_unknown_arg,
        diag::LLVMStr(arg->getAsString(*compilerInputArgList)));

    // TODO: Good for now. But, you want to print out all and check for diag
    // errors
    return Status::Error();
  }

  if (compiler.GetDiags().HasError()) {
    return Status::Error();
  }
  auto status = ParseCompilerOptions(
      *compilerInputArgList, langOpts, compilerOpts, compiler.GetDiags(),
      nullptr); // TODO: Pass MemoryBuffers in ParseCommandLine

  if (status.IsError()) {
    status.SetHasCompletion();
  }
  return status;
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

bool CompilerInvocation::ShouldSetupClang() {
  return GetMainAction().IsAny(ActionKind::EmitIRBefore,
                               ActionKind::EmitIRAfter, ActionKind::EmitBC,
                               ActionKind::EmitAssembly, ActionKind::EmitObject,
                               ActionKind::EmitLibrary);
}

Status CompilerInvocation::SetupClang(llvm::ArrayRef<const char *> argv,
                                      const char *arg0) {

  if (clangContext->Setup(argv, arg0).IsError()) {
    return Status::Error();
  }
  return Status();
}