#ifndef STONE_BASIC_LLVMINIT_H
#define STONE_BASIC_LLVMINIT_H

#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/ManagedStatic.h"
#include "llvm/Support/PrettyStackTrace.h"
#include "llvm/Support/Signals.h"
#include "llvm/Support/TargetSelect.h"

#define START_LLVM_INIT(argc, argv) llvm::InitLLVM _INITIALIZE_LLVM(argc, argv)

#define FINISH_LLVM_INIT()                                                     \
  do {                                                                         \
    llvm::InitializeAllTargets();                                              \
    llvm::InitializeAllTargetMCs();                                            \
    llvm::InitializeAllAsmPrinters();                                          \
    llvm::InitializeAllAsmParsers();                                           \
    llvm::InitializeAllDisassemblers();                                        \
    llvm::InitializeAllTargetInfos();                                          \
  } while (0)

#endif // STONE_BASIC_LLVMINIT_H