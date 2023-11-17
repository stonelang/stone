#include "stone/Compile/CompilerInvocation.h"
#include "stone/Compile/CompilerOptionsConverter.h"
#include "stone/Compile/Compiler.h"
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
    : compiler(compiler) {
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
  unsigned excludedFlagsBitmask;
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
  }
}