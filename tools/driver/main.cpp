#include "stone/Driver/Main.h"
#include "stone/Basic/LLVMInit.h"
#include "stone/Basic/MainExecutablePath.h"

#include "llvm/ADT/Triple.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/Process.h"
#include "llvm/Support/Program.h"
#include "llvm/Support/StringSaver.h"
#include "llvm/Support/TargetSelect.h"

using namespace stone;

int main(int argc, const char **args) {
  START_LLVM_INIT(argc, args);

  llvm::SmallVector<const char *, 256> argsToExpand(args, args + argc);
  llvm::BumpPtrAllocator ptrAlloc;
  llvm::StringSaver strSaver(ptrAlloc);
  llvm::cl::ExpandResponseFiles(
      strSaver,
      llvm::Triple(llvm::sys::getProcessTriple()).isOSWindows()
          ? llvm::cl::TokenizeWindowsCommandLine
          : llvm::cl::TokenizeGNUCommandLine,
      argsToExpand);

  llvm::ArrayRef<const char *> argv(argsToExpand);
  return stone::Main(
      llvm::makeArrayRef(argv.data() + 1, argv.data() + argv.size()), argv[0],
      (void *)(intptr_t)stone::GetMainExecutablePath);
}
