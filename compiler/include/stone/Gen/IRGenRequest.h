#ifndef STONE_GEN_CODEGEN_INVOCATION_H
#define STONE_GEN_CODEGEN_INVOCATION_H

#include "stone/Basic/CodeGenOptions.h"
#include "stone/Basic/Memory.h"
#include "stone/Basic/PrimaryFileSpecificPaths.h"
#include "stone/Basic/STDAlias.h"
#include "stone/Basic/Status.h"
#include "stone/Basic/TargetOptions.h"
#include "stone/Core.h"
#include "stone/Syntax/Module.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {
class raw_pwrite_stream;
class GlobalVariable;
class MemoryBuffer;
class Module;
class TargetOptions;
class TargetMachine;
} // namespace llvm

namespace stone {

class ASTContext;
class ModuleDecl;
class MouleFile;
class CodeGenOptions;
class CodeGenListener;

class IRGenRequest final {

  const CodeGenOptions &codeGenOpts;
  ASTContext &astContext;
  const PrimaryFileSpecificPaths primaryFileSpecificPaths;
  ModuleDeclOrModuleFile moduleOrFile;
  llvm::StringRef moduleName;
  llvm::GlobalVariable *outModuleHash;
  llvm::ArrayRef<std::string> parallelOutputFilenames;

public:
  IRGenRequest(const CodeGenOptions &codeGenOpts, ModuleDecl *moduleDecl,
               const llvm::StringRef moduleName, ASTContext &astContext,
               const PrimaryFileSpecificPaths primaryFileSpecificPaths,
               llvm::ArrayRef<std::string> parallelOutputFilenames,
               llvm::GlobalVariable *outModuleHash = nullptr);

  IRGenRequest(const CodeGenOptions &codeGenOpts, ModuleFile *moduleFile,
               const llvm::StringRef moduleName, ASTContext &astContext,
               const PrimaryFileSpecificPaths primaryFileSpecificPaths,
               llvm::ArrayRef<std::string> parallelOutputFilenames,
               llvm::GlobalVariable *outModuleHash = nullptr);

  ~IRGenRequest();

public:
  const CodeGenOptions &GetCodeGenOptions() const { return codeGenOpts; }
  ASTContext &GetASTContext() const { return astContext; }

  llvm::GlobalVariable *GetOutModuleHash() { return outModuleHash; }
  void SetOutModuleHash(llvm::GlobalVariable *hash) { outModuleHash = hash; }

  const PrimaryFileSpecificPaths &GetPrimaryFileSpecificPaths() {
    return primaryFileSpecificPaths;
  }

  llvm::StringRef GetModuleName() { return moduleName; }
  llvm::TinyPtrVector<ModuleFile *> GetFiles() const;
  ModuleDecl *GetParentModule() const;

  // TODO: May not want to be const.
  ModuleFile *GetPrimaryFile() const {
    return moduleOrFile.get<ModuleFile *>();
  }
  // bool IsForWholeModule() {
  //   return moduleOrFile.dyn_cast<SourceFile *>() == nullptr;
  // }

  SourceFile *GetPrimarySourceFile() {
    return llvm::dyn_cast_or_null<SourceFile>(
        moduleOrFile.dyn_cast<ModuleFile *>());
  }

  ASTContext &GetASTContext() { return astContext; }

public:
  static IRGenRequest
  ForModule(const CodeGenOptions &codeGenOpts, ModuleDecl *moduleDecl,
            const llvm::StringRef moduleName, ASTContext &astContext,
            const PrimaryFileSpecificPaths primaryFileSpecificPaths,
            llvm::ArrayRef<std::string> parallelOutputFilenames,
            llvm::GlobalVariable *outModuleHash = nullptr);

  static IRGenRequest
  ForFile(const CodeGenOptions &codeGenOpts, ModuleFile *moduleFile,
          const llvm::StringRef moduleName, ASTContext &astContext,
          const PrimaryFileSpecificPaths primaryFileSpecificPaths,
          llvm::GlobalVariable *outModuleHash = nullptr);
};

} // namespace stone

#endif
