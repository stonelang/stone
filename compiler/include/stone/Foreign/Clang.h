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
  Safe<ClangModuleImporter> clangModuleImporter;
  Safe<clang::CompilerInstance> compilerInstance;

public:
  Clang();

public:
  ClangModuleImporter &GetModuleImporter() { return *clangModuleImporter; }
  clang::CompilerInstance &GetInstance() { return *compilerInstance; }
};

} // namespace stone
#endif
