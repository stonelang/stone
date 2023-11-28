#ifndef STONE_GEN_CODEGEN_INVOCATION_H
#define STONE_GEN_CODEGEN_INVOCATION_H

#include "stone/Basic/CodeGenOptions.h"
#include "stone/Basic/Mem.h"
#include "stone/Basic/PrimaryFileSpecificPaths.h"
#include "stone/Basic/STDAlias.h"
#include "stone/Basic/Status.h"
#include "stone/Basic/TargetOptions.h"
#include "stone/Public.h"
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

class IRCodeGenRequest final {

  const CodeGenOptions &codeGenOpts;
  ASTContext &astContext;
  MemoryContext &memContext;
  const PrimaryFileSpecificPaths primaryFileSpecificPaths;
  ModuleDeclOrModuleFile moduleOrFile;
  llvm::StringRef moduleName;
  llvm::GlobalVariable *outModuleHash;

  CodeGenListener *listener;

public:
  IRCodeGenRequest(const CodeGenOptions &codeGenOpts, ModuleDecl *moduleDecl,
                   const llvm::StringRef moduleName, ASTContext &astContext,
                   MemoryContext &memContext,
                   const PrimaryFileSpecificPaths primaryFileSpecificPaths,
                   llvm::GlobalVariable *outModuleHash = nullptr);

  IRCodeGenRequest(const CodeGenOptions &codeGenOpts, ModuleFile *moduleFile,
                   const llvm::StringRef moduleName, ASTContext &astContext,
                   MemoryContext &memContext,
                   const PrimaryFileSpecificPaths primaryFileSpecificPaths,
                   llvm::GlobalVariable *outModuleHash = nullptr);

  ~IRCodeGenRequest();

public:
  const CodeGenOptions &GetCodeGenOptions() const { return codeGenOpts; }
  ASTContext &GetASTContext() { return astContext; }
  MemoryContext &GetMemoryContext() { return memContext; }

  llvm::GlobalVariable *GetOutModuleHash() { return outModuleHash; }
  void SetOutModuleHash(llvm::GlobalVariable *hash) { outModuleHash = hash; }

  const PrimaryFileSpecificPaths &GetPrimaryFileSpecificPaths() {
    return primaryFileSpecificPaths;
  }

  llvm::ArrayRef<ModuleFile *> GetFiles() const;
  ModuleDecl *GetParentModule();

  void SetCodeGenListener(CodeGenListener *inputListener) {
    listener = inputListener;
  }
  CodeGenListener *GetCodeGenListener() { return listener; }

  // TODO: This code is also in NativeCodegen
  llvm::CodeGenFileType GetCodeGenFileType() {
    return (GetCodeGenOptions().codeGenOutputKind ==
                    CodeGenOutputKind::NativeAssembly
                ? llvm::CGFT_AssemblyFile
                : llvm::CGFT_ObjectFile);
  }

public:
  static IRCodeGenRequest
  ForModule(const CodeGenOptions &codeGenOpts, ModuleDecl *moduleDecl,
            const llvm::StringRef moduleName, ASTContext &astContext,
            MemoryContext &memContext,
            const PrimaryFileSpecificPaths primaryFileSpecificPaths,
            llvm::GlobalVariable *outModuleHash = nullptr);

  static IRCodeGenRequest
  ForFile(const CodeGenOptions &codeGenOpts, ModuleFile *moduleFile,
          const llvm::StringRef moduleName, ASTContext &astContext,
          MemoryContext &memContext,
          const PrimaryFileSpecificPaths primaryFileSpecificPaths,
          llvm::GlobalVariable *outModuleHash = nullptr);
};

} // namespace stone

#endif
