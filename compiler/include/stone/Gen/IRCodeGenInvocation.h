#ifndef STONE_GEN_CODEGEN_INVOCATION_H
#define STONE_GEN_CODEGEN_INVOCATION_H

#include "stone/Basic/CodeGenOptions.h"
#include "stone/Basic/STDAlias.h"
#include "stone/Basic/Status.h"
#include "stone/Basic/TargetOptions.h"
#include "stone/Public.h"

#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
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

class IRCodeGenInvocation final {

  const CodeGenOptions &codeGenOpts;
  ASTContext &astContext;
  const PrimaryFileSpecificPaths primaryFileSpecificPaths;
  ModuleDeclOrModuleFile moduleOrFile;
  llvm::GlobalVariable *outModuleHash;

  CodeGenListener *listener;

public:
  IRCodeGenInvocation(const CodeGenOptions &codeGenOpts, ModuleDecl &moduleDecl,
                      const llvm::StringRef moduleName, ASTContext &astContext,
                      const PrimaryFileSpecificPaths primaryFileSpecificPaths,
                      llvm::GlobalVariable *outModuleHash = nullptr);

  IRCodeGenInvocation(const CodeGenOptions &codeGenOpts, ModuleFile &moduleFile,
                      const llvm::StringRef moduleName, ASTContext &astContext,
                      const PrimaryFileSpecificPaths primaryFileSpecificPaths,
                      llvm::GlobalVariable *outModuleHash = nullptr);

  ~IRCodeGenInvocation();

public:
  const CodeGenOptions &GetCodeGenOptions() const { return codeGenOpts; }
  ASTContext &GetASTContext() { return astContext; }

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

  llvm::CodeGenFileType GetCodeGenFileType() {
    return (GetCodeGenOptions().codeGenOutputKind ==
                    CodeGenOutputKind::NativeAssembly
                ? llvm::CGFT_AssemblyFile
                : llvm::CGFT_ObjectFile);
  }

public:
  static IRCodeGenInvocation
  ForModule(const CodeGenOptions &codeGenOpts, ModuleDecl *moduleDecl,
            const llvm::StringRef moduleName, ASTContext &astContext,
            const PrimaryFileSpecificPaths primaryFileSpecificPaths,
            llvm::GlobalVariable *outModuleHash = nullptr);

  static IRCodeGenInvocation
  ForFile(const CodeGenOptions &codeGenOpts, ModuleFile *moduleFile,
          const llvm::StringRef moduleName, ASTContext &astContext,
          const PrimaryFileSpecificPaths primaryFileSpecificPaths,
          llvm::GlobalVariable *outModuleHash = nullptr);
};

} // namespace stone

#endif
