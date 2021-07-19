#include "stone/Basic/Basic.h"
#include "llvm/Support/Host.h"

using namespace stone;

Basic::Basic()
    : fm(fsOpts), de(diagOpts), cos(llvm::outs()),
      targetTriple(llvm::sys::getDefaultTargetTriple()) {}

Basic::~Basic() {}

void Basic::Panic() { assert(false && "Compiler cannot continue"); }