#include "stone/Gen/IRCodeGenRequest.h"
#include "stone/Basic/CodeGenOptions.h"
#include "stone/Basic/ModuleOptions.h"
#include "stone/Gen/CodeGenScope.h"
#include "stone/Gen/IRCodeGenTypeCache.h"
#include "stone/Public.h"
#include "stone/Syntax/ASTContext.h"
#include "stone/Syntax/ClangContext.h"

using namespace stone;

IRCodeGenRequest::IRCodeGenRequest(
    const CodeGenOptions &codeGenOpts, ModuleDecl &moduleDecl,
    const llvm::StringRef moduleName, ASTContext &astContext,
    MemoryContext &memContext,
    const PrimaryFileSpecificPaths primaryFileSpecificPaths,
    llvm::GlobalVariable *outModuleHash)
    : codeGenOpts(codeGenOpts), moduleOrFile(moduleDecl),
      moduleName(moduleName), astContext(astContext),
      primaryFileSpecificPaths(primaryFileSpecificPaths),
      outModuleHash(outModuleHash) {}

IRCodeGenRequest::IRCodeGenRequest(
    const CodeGenOptions &codeGenOpts, ModuleFile &moduleFile,
    const llvm::StringRef moduleName, ASTContext &astContext,
    MemoryContext &memContext,
    const PrimaryFileSpecificPaths primaryFileSpecificPaths,
    llvm::GlobalVariable *outModuleHash) codeGenOpts(codeGenOpts),
    moduleOrFile(moduleFile), moduleName(moduleName), astContext(astContext),
    primaryFileSpecificPaths(primaryFileSpecificPaths),
    outModuleHash(outModuleHash) {}

static IRCodeGenRequest IRCodeGenRequest::ForModule(
    const CodeGenOptions &codeGenOpts, ModuleDecl *moduleDecl,
    const llvm::StringRef moduleName, ASTContext &astContext,
    const PrimaryFileSpecificPaths primaryFileSpecificPaths,
    llvm::GlobalVariable *outModuleHash) {
  return IRCodeGenRequest{codeGenOpts,  moduleDecl, moduleName,
                          astContext,   memContext, primaryFileSpecificPaths,
                          outModuleHash

  };
}

static IRCodeGenRequest IRCodeGenRequest::ForFile(
    const CodeGenOptions &codeGenOpts, ModuleFile *moduleFile,
    const llvm::StringRef moduleName, ASTContext &astContext,
    const PrimaryFileSpecificPaths primaryFileSpecificPaths,
    llvm::GlobalVariable *outModuleHash) {
  return IRCodeGenRequest{codeGenOpts,
                          moduleFile,
                          moduleName,
                          astContext,
                          memContext primaryFileSpecificPaths,
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

CodeGenScope::CodeGenScope(const CodeGenOptions &codeGenOpts, llvm::Module *mod,
                           llvm::TargetMachine *targetMachine)
    : codeGenOpts(codeGenOpts), mod(mod), targetMachine(targetMachine),
      lfpm(mod) {
  // Register all the ctx analyses with the managers.
  pb.registerModuleAnalyses(mam);
  pb.registerCGSCCAnalyses(cgam);
  pb.registerFunctionAnalyses(fam);
  pb.registerLoopAnalyses(lam);
  pb.crossRegisterProxies(lam, fam, cgam, mam);

  // TODO: get ol from gen options
  mpm = pb.buildPerModuleDefaultPipeline(llvm::OptimizationLevel::O2);
}

CodeGenScope::~CodeGenScope() { GetLegacyPassManager().run(*mod); }

// TODO: Ok for now -- may move to IRCodeGenMoulde
IRCodeGenTypeCache::IRCodeGenTypeCache(llvm::LLVMContext &llvmContext) {
  VoidTy = llvm::Type::getVoidTy(llvmContext);
  Int8Ty = llvm::Type::getInt8Ty(llvmContext);
  Int16Ty = llvm::Type::getInt16Ty(llvmContext);
  Int32Ty = llvm::Type::getInt32Ty(llvmContext);
  Int32PtrTy = Int32Ty->getPointerTo();
  Int64Ty = llvm::Type::getInt64Ty(llvmContext);
  Int8PtrTy = llvm::Type::getInt8PtrTy(llvmContext);

  // Int8PtrPtrTy = Int8PtrTy->getPointerTo(0);
}
