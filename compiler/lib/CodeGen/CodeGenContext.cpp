#include "stone/Gen/CodeGenContext.h"
#include "stone/Basic/CodeGenOptions.h"
#include "stone/Basic/ModuleOptions.h"
#include "stone/Syntax/ClangContext.h"
#include "stone/Public.h"

using namespace stone;

// using namespace stone;
// using namespace stone::codegen;

// CodeGenContext::CodeGenContext(const CodeGenOptions &genOpts,
//                                llvm::LLVMContext &llvmContext,
//                                const stone::TargetOptions &targetOpts,
//                                const LangContext &langContext,
//                                ClangContext &clangContext,
//                                llvm::GlobalVariable **outModuleHash)
//     : CodeGenContext(
//           genOpts, targetOpts, langContext, clangContext,
//           std::make_unique<llvm::Module>(moduleOpts.moduleName, llvmContext),
//           outModuleHash) {}

// CodeGenContext::CodeGenContext(const CodeGenOptions &genOpts,
//                                llvm::LLVMContext &llvmContext,
//                                const stone::TargetOptions &targetOpts,
//                                const LangContext &langContext,
//                                ClangContext &clangContext,
//                                std::unique_ptr<llvm::Module> llvmMod,
//                                llvm::GlobalVariable **outModuleHash)
//     : codeGenOpts(codeGenOpts), llvmContext(llvmContext),
//       llvmModule(llvmMod.release()) {

//   // Register all the ctx analyses with the managers.
//   pb.registerModuleAnalyses(mam);
//   pb.registerCGSCCAnalyses(cgam);
//   pb.registerFunctionAnalyses(fam);
//   pb.registerLoopAnalyses(lam);
//   pb.crossRegisterProxies(lam, fam, cgam, mam);

//   // TODO: get ol from gen options
//   mpm = pb.buildPerModuleDefaultPipeline(llvm::OptimizationLevel::O2);
// }

// std::unique_ptr<llvm::TargetMachine>
// stone::CreateTargetMachine(CodeGenOptions &opts, ASTContext &ac) {}
