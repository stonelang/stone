#include "stone/Compile/CompilerCommandLine.h"
#include "stone/Compile/CompilerOptionsConverter.h"

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
  SetTargetTriple(llvm::sys::getDefaultTargetTriple());
}

void CompilerContext::SetTargetTriple(llvm::StringRef triple) {}
void CompilerContext::SetTargetTriple(const llvm::Triple &Triple) {}
void CompilerContext::SetWorkingDirectory() {}

CompilerCommandLine::CompilerCommandLine() {}

std::unique_ptr<CompilerContext>
CompilerCommandLine::Parse(llvm::ArrayRef<const char *> args) {
  auto compilerContext = std::make_unique<CompilerContext>();
  return compilerContext;
}

Status CompilerCommandLine::ParseCompilerAction(llvm::opt::InputArgList &args) {

}
Status CompilerCommandLine::ParseCompilerOptions(llvm::opt::InputArgList &args,
                                                 CompilerMemoryBuffers *buffers) {}

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
