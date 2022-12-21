#include "stone/Foreign/Clang.h"

using namespace stone;

Clang::Clang() : compilerInstance(new clang::CompilerInstance()) {}

bool Clang::ComputeOptions(llvm::ArrayRef<const char *> argv,
                           const char *arg0) {
  return false;
}
