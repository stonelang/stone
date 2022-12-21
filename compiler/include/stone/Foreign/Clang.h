#ifndef STONE_FOREIGN_CLANG_H
#define STONE_FOREIGN_CLANG_H

#include "stone/Basic/Mem.h"
#include "stone/Foreign/ClangModuleImporter.h"

#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/CompilerInvocation.h"

#include "llvm/ADT/ArrayRef.h"

using namespace stone::mem;

namespace stone {

class Clang final {
  Safe<ClangModuleImporter> moduleImporter;
  Safe<clang::CompilerInstance> compilerInstance;

public:
  Clang();

public:
  ClangModuleImporter &GetModuleImporter() { return *moduleImporter; }
  clang::CompilerInstance &GetInstance() { return *compilerInstance; }

public:
  bool ComputeOptions(llvm::ArrayRef<const char *> argv, const char *arg0);
};

} // namespace stone
#endif
