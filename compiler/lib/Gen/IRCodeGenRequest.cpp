#include "stone/Gen/IRCodeGenRequest.h"
#include "stone/Basic/CodeGenOptions.h"
#include "stone/Basic/ModuleOptions.h"
#include "stone/Gen/IRCodeGenTypeCache.h"
#include "stone/Public.h"
#include "stone/Syntax/ASTContext.h"
#include "stone/Syntax/ClangContext.h"

using namespace stone;

IRCodeGenRequest::IRCodeGenRequest(
    const CodeGenOptions &codeGenOpts, ModuleDecl *moduleDecl,
    const llvm::StringRef moduleName, ASTContext &astContext,
    MemoryContext &memContext,
    const PrimaryFileSpecificPaths primaryFileSpecificPaths,
    llvm::GlobalVariable *outModuleHash)
    : codeGenOpts(codeGenOpts), moduleOrFile(moduleDecl),
      moduleName(moduleName), astContext(astContext), memContext(memContext),
      primaryFileSpecificPaths(primaryFileSpecificPaths),
      outModuleHash(outModuleHash) {}

IRCodeGenRequest::IRCodeGenRequest(
    const CodeGenOptions &codeGenOpts, ModuleFile *moduleFile,
    const llvm::StringRef moduleName, ASTContext &astContext,
    MemoryContext &memContext,
    const PrimaryFileSpecificPaths primaryFileSpecificPaths,
    llvm::GlobalVariable *outModuleHash)
    : codeGenOpts(codeGenOpts), moduleOrFile(moduleFile),
      moduleName(moduleName), astContext(astContext), memContext(memContext),
      primaryFileSpecificPaths(primaryFileSpecificPaths),
      outModuleHash(outModuleHash) {}

IRCodeGenRequest IRCodeGenRequest::ForModule(
    const CodeGenOptions &codeGenOpts, ModuleDecl *moduleDecl,
    const llvm::StringRef moduleName, ASTContext &astContext,
    MemoryContext &memContext,
    const PrimaryFileSpecificPaths primaryFileSpecificPaths,
    llvm::GlobalVariable *outModuleHash) {
  return IRCodeGenRequest{codeGenOpts,  moduleDecl, moduleName,
                          astContext,   memContext, primaryFileSpecificPaths,
                          outModuleHash};
}

IRCodeGenRequest IRCodeGenRequest::ForFile(
    const CodeGenOptions &codeGenOpts, ModuleFile *moduleFile,
    const llvm::StringRef moduleName, ASTContext &astContext,
    MemoryContext &memContext,
    const PrimaryFileSpecificPaths primaryFileSpecificPaths,
    llvm::GlobalVariable *outModuleHash) {
  return IRCodeGenRequest{codeGenOpts,  moduleFile, moduleName,
                          astContext,   memContext, primaryFileSpecificPaths,
                          outModuleHash

  };
}

// llvm::ArrayRef<ModuleFile *> IRCodeGenRequest::GetFiles() const {}

// ModuleDecl *IRCodeGenRequest::GetParentModule() {}

IRCodeGenRequest::~IRCodeGenRequest() {}

// Maybe? Make ASTAllocation
// IRCodeGenRequest *IRCodeGenRequest::Create(const CodeGenOptions
// &codeGenOpts,
//                                        const llvm::StringRef moduleName,
//                                        ASTContext &astContext,
//                                        llvm::GlobalVariable *outModuleHash) {

//   return new (astContext) IRCodeGenRequest(codeGenOpts, moduleName,
//   astContext, outModuleHash);

// }

// Safe<llvm::TargetMachine> IRCodeGenRequest::CreateTargetMachine() {

//   // clang::TargetInfo &targetInfo =
//   // GetClangContext().GetInstance().getTarget();

//   // // Setup the empty module
//   // GetLLVMModule().setTargetTriple(targetInfo.getTriple().getTriple());
//   // GetLLVMModule().setDataLayout(targetInfo.getDataLayoutString());

//   // const auto &sdkVersion = targetInfo.getSDKVersion();

//   // if (!sdkVersion.empty()) {
//   //   cgc.GetLLVMModule().setSDKVersion(sdkVersion);
//   // }
//   // if (const auto *tvt = targetInfo.getDarwinTargetVariantTriple()) {
//   //   cgc.GetModule().setDarwinTargetVariantTriple(tvt->getTriple());
//   // }

//   // if (auto TVSDKVersion = targetInfo.getDarwinTargetVariantSDKVersion()) {
//   //   cgc.GetModule().setDarwinTargetVariantSDKVersion(*TVSDKVersion);
//   // }
// }
