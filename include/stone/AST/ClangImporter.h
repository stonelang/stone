#ifndef STONE_AST_CLANG_IMPORTER_H
#define STONE_AST_CLANG_IMPORTER_H

#include "stone/Basic/Basic.h"
#include "stone/Basic/Status.h"

#include "clang/Basic/TargetInfo.h"
#include "clang/CodeGen/ModuleBuilder.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/CompilerInvocation.h"
#include "clang/Lex/Preprocessor.h"

#include "llvm/ADT/ArrayRef.h"

namespace stone {

class ClangModuleImporterOptions final {
public:
};

class ClangModuleLoader {
public:
};

class ClangModuleImporter : public ClangModuleLoader {

public:
  ClangModuleImporter();
};

class ClangImporter final : public ClangModuleImporter {

  clang::FileManager fileMgr;
  clang::FileSystemOptions fileSystemOpts;
  std::unique_ptr<clang::CompilerInstance> clangInstance;

public:
  ClangImporter();

public:
  bool Setup(llvm::ArrayRef<const char *> argv, const char *arg0);

public:
  clang::FileSystemOptions &GetFileSystemOptions() { return fileSystemOpts; }
  const clang::FileSystemOptions &GetFileSystemOptions() const {
    return fileSystemOpts;
  }

  clang::FileManager &GetFileMgr() { return fileMgr; }
  clang::CompilerInstance &GetClangInstance() { return *clangInstance; }

  std::unique_ptr<clang::CodeGenerator>
  CreateCodeGenerator(llvm::LLVMContext &llvmContext,
                      llvm::StringRef moduleName);
};

} // namespace stone
#endif