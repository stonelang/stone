#include "stone/Gen/IRGenRequest.h"
#include "stone/Basic/CodeGenOptions.h"
#include "stone/Basic/ModuleOptions.h"
#include "stone/Gen/IRGenTypeCache.h"
#include "stone/Syntax/ASTContext.h"
#include "stone/Syntax/ClangContext.h"

using namespace stone;

IRGenRequest::IRGenRequest(
    const CodeGenOptions &codeGenOpts, ModuleDecl *moduleDecl,
    const llvm::StringRef moduleName, ASTContext &astContext,
    const PrimaryFileSpecificPaths primaryFileSpecificPaths,
    llvm::ArrayRef<std::string> parallelOutputFilenames,
    llvm::GlobalVariable *outModuleHash)
    : codeGenOpts(codeGenOpts), moduleOrFile(moduleDecl),
      moduleName(moduleName), astContext(astContext),
      primaryFileSpecificPaths(primaryFileSpecificPaths),
      outModuleHash(outModuleHash) {}

IRGenRequest::IRGenRequest(
    const CodeGenOptions &codeGenOpts, ModuleFile *moduleFile,
    const llvm::StringRef moduleName, ASTContext &astContext,
    const PrimaryFileSpecificPaths primaryFileSpecificPaths,
    llvm::ArrayRef<std::string> parallelOutputFilenames,
    llvm::GlobalVariable *outModuleHash)
    : codeGenOpts(codeGenOpts), moduleOrFile(moduleFile),
      moduleName(moduleName), astContext(astContext),
      primaryFileSpecificPaths(primaryFileSpecificPaths),
      outModuleHash(outModuleHash) {}

IRGenRequest IRGenRequest::ForModule(
    const CodeGenOptions &codeGenOpts, ModuleDecl *moduleDecl,
    const llvm::StringRef moduleName, ASTContext &astContext,
    const PrimaryFileSpecificPaths primaryFileSpecificPaths,
    llvm::ArrayRef<std::string> parallelOutputFilenames,
    llvm::GlobalVariable *outModuleHash) {
  return IRGenRequest{codeGenOpts,
                      moduleDecl,
                      moduleName,
                      astContext,
                      primaryFileSpecificPaths,
                      parallelOutputFilenames,
                      outModuleHash};
}

IRGenRequest
IRGenRequest::ForFile(const CodeGenOptions &codeGenOpts, ModuleFile *moduleFile,
                      const llvm::StringRef moduleName, ASTContext &astContext,
                      const PrimaryFileSpecificPaths primaryFileSpecificPaths,
                      llvm::GlobalVariable *outModuleHash) {
  return IRGenRequest{codeGenOpts,
                      moduleFile,
                      moduleName,
                      astContext,
                      primaryFileSpecificPaths,
                      {},
                      outModuleHash

  };
}

// llvm::ArrayRef<ModuleFile *> IRGenRequest::GetFiles() const {}

// ModuleDecl *IRGenRequest::GetParentModule() {}

IRGenRequest::~IRGenRequest() {}

llvm::TinyPtrVector<ModuleFile *> IRGenRequest::GetFiles() const {
  // If we've been asked to emit a specific set of symbols, we don't emit any
  // whole files.
  // if (SymbolsToEmit)
  //   return {};

  // For a whole module, we emit IR for all files.
  if (auto *mod = moduleOrFile.dyn_cast<ModuleDecl *>()) {
    return llvm::TinyPtrVector<ModuleFile *>(mod->GetFiles());
  }
  // For a primary file, we emit IR for both it and potentially its
  // SynthesizedFileUnit.
  auto *primary = GetPrimaryFile();
  llvm::TinyPtrVector<ModuleFile *> files;
  files.push_back(primary);

  return files;
}

ModuleDecl *IRGenRequest::GetParentModule() const {
  if (auto *file = moduleOrFile.dyn_cast<ModuleFile *>()) {
    return file->GetParentModule();
  }
  return moduleOrFile.get<ModuleDecl *>();
}

// Maybe? Make MemoryAllocation
// IRGenRequest *IRGenRequest::Create(const CodeGenOptions
// &codeGenOpts,
//                                        const llvm::StringRef moduleName,
//                                        ASTContext &astContext,
//                                        llvm::GlobalVariable *outModuleHash) {

//   return new (astContext) IRGenRequest(codeGenOpts, moduleName,
//   astContext, outModuleHash);

// }

// Safe<llvm::TargetMachine> IRGenRequest::CreateTargetMachine() {

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
