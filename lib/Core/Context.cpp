#include "stone/Core/Context.h"
#include "stone/Core/CoreDiagnostic.h"

#include "llvm/Support/Host.h"

using namespace stone;

Context::Context()
    : fm(GetFileSystemOptions()), de(GetDiagOptions()), cos(llvm::outs()),
      defaultTargetTriple(llvm::sys::getDefaultTargetTriple()) {}

Context::~Context() {}

// TODO: Remove
void stone::Panic() { assert(false && "Compiler cannot continue!"); }
void stone::Panic(const char *msg) { llvm_unreachable(msg); }
