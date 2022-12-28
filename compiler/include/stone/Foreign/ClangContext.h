#ifndef STONE_FOREIGN_CLANG_H
#define STONE_FOREIGN_CLANG_H

#include "stone/Basic/STDAlias.h"
#include "stone/Foreign/ClangModuleImporter.h"

#include "clang/Basic/TargetInfo.h"
#include "clang/CodeGen/ModuleBuilder.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/CompilerInvocation.h"

#include "llvm/ADT/ArrayRef.h"

namespace stone {

class ClangContext final : public ClangModuleImporter {

  Safe<clang::CompilerInstance> clangInstance;
  Safe<clang::CodeGenerator> clangCodeGen;

public:
  ClangContext();

public:
  clang::CompilerInstance &GetInstance() { return *clangInstance; }
  clang::CodeGenerator &GetCodeGen() { return *clangCodeGen; }
};

} // namespace stone
#endif
