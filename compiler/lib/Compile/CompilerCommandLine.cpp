
#include "stone/Compile/Compiler.h"
#include "stone/Compile/CompilerOptions.h"
#include "stone/Compile/CompilerOptionsConverter.h"
#include "stone/Diag/CompilerDiagnostic.h"

#include "clang/Basic/Stack.h"
#include "clang/Basic/TargetOptions.h"
#include "clang/CodeGen/ObjectFilePCHContainerOperations.h"
#include "clang/Config/config.h"
#include "clang/Driver/DriverDiagnostic.h"
#include "clang/Driver/Options.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/CompilerInvocation.h"
#include "clang/Frontend/FrontendDiagnostic.h"
#include "clang/Frontend/TextDiagnosticBuffer.h"
#include "clang/Frontend/TextDiagnosticPrinter.h"
#include "clang/Frontend/Utils.h"

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

CompilerCommandLine::CompilerCommandLine() {}

std::unique_ptr<CompilerContext> CompilerCommandLine::Parse() {
  auto compilerContext = std::make_unique<CompilerContext>();

  return compilerContext;
}

Status CompilerCommandLine::ParseCompilerAction(llvm::opt::InputArgList &args) {

}
Status CompilerCommandLine::ParseCompilerOptions(llvm::opt::InputArgList &args,
                                                 MemoryBuffers *buffers) {}

Status CompilerCommandLine::ParseLangOptions(llvm::opt::InputArgList &args) {}

Status
CompilerCommandLine::ParseTypeCheckerOptions(llvm::opt::InputArgList &args) {}

Status
CompilerCommandLine::ParseSearchPathOptions(llvm::opt::InputArgList &args) {}

Status CompilerCommandLine::ParseCodeGenOptions(llvm::opt::InputArgList &args) {

}
Status CompilerCommandLine::ParseTargetOptions(llvm::opt::InputArgList &args) {}

llvm::StringRef
CompilerCommandLine::ParseWorkDirectory(const llvm::opt::InputArgList &args) {}
