#ifndef STONE_FOREIGN_CLANG_H
#define STONE_FOREIGN_CLANG_H

#include "stone/Basic/STDAlias.h"
#include "stone/Basic/Status.h"
#include "stone/Syntax/ClangModuleImporter.h"

#include "clang/Basic/TargetInfo.h"
#include "clang/CodeGen/ModuleBuilder.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/CompilerInvocation.h"
#include "clang/Lex/Preprocessor.h"

#include "llvm/ADT/ArrayRef.h"

namespace stone {

class ClangContext final : public ClangModuleImporter {

  std::unique_ptr<clang::CompilerInstance> clangInstance;
  std::unique_ptr<clang::CodeGenerator> clangCodeGen;

public:
  ClangContext();

public:
  Status Setup(llvm::ArrayRef<const char *> argv, const char *arg0);

public:
  clang::CompilerInstance &GetInstance() { return *clangInstance; }
  clang::CodeGenerator &GetCodeGen() { return *clangCodeGen; }
};

} // namespace stone
#endif
