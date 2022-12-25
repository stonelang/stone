#ifndef STONE_FOREIGN_CLANG_H
#define STONE_FOREIGN_CLANG_H

#include "stone/Basic/STDAlias.h"
#include "stone/Foreign/ClangModuleImporter.h"

#include "clang/CodeGen/ModuleBuilder.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/CompilerInvocation.h"

#include "llvm/ADT/ArrayRef.h"

namespace stone {

class ClangContext final {
  Safe<ClangModuleImporter> clangModuleImporter;
  Safe<clang::CompilerInstance> clangInstance;
  Safe<clang::CodeGenerator> clangCodeGen;

public:
  ClangContext();

public:
  ClangModuleImporter &GetModuleImporter() { return *clangModuleImporter; }
  clang::CompilerInstance &GetInstance() { return *clangInstance; }
  clang::CodeGenerator &GetCodeGen() { return *clangCodeGen; }
};

} // namespace stone
#endif
