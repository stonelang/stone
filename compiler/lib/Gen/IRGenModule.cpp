#include "stone/Gen/IRGenModule.h"
#include "stone/Basic/CodeGenOptions.h"
#include "stone/Core.h"
#include "stone/Gen/IRGenABI.h"
#include "stone/Syntax/Decl.h"

using namespace stone;

IRGen::IRGen(const CodeGenOptions &codeGenOpts, ASTContext &astContext)
    : codeGenOpts(codeGenOpts), astContext(astContext),
      llvmContext(new llvm::LLVMContext()),
      llvmTargetMachine(stone::CreateTargetMachine(codeGenOpts)) {}

llvm::Triple IRGen::GetEffectiveClangTriple() {
  return llvm::Triple(astContext.GetClangContext()
                          .GetInstance()
                          .getTarget()
                          .getTargetOpts()
                          .Triple);
}

const llvm::StringRef IRGen::GetClangDataLayoutString() {
  return astContext.GetClangContext()
      .GetInstance()
      .getTarget()
      .getDataLayoutString();
}

static clang::CodeGenerator *CreateClangCodeGen(IRGen &irGen,
                                                llvm::StringRef moduleName) {

  auto &clangInstance = irGen.GetASTContext().GetClangContext().GetInstance();
  auto &clangASTContext = clangInstance.getASTContext();
  auto &clangCodeGenOpts = clangInstance.getCodeGenOpts();

  clangCodeGenOpts.OptimizationLevel =
      irGen.GetCodeGenOptions().ShouldOptimize() ? 3 : 0;

  clangCodeGenOpts.CoverageMapping = false;

  auto &vfs = clangInstance.getVirtualFileSystem();
  auto &headerSearchOpts = clangInstance.getPreprocessor()
                               .getHeaderSearchInfo()
                               .getHeaderSearchOpts();

  auto &preprocessorOpts =
      clangInstance.getPreprocessor().getPreprocessorOpts();
  auto *clangCodeGen = clang::CreateLLVMCodeGen(
      clangInstance.getDiagnostics(), moduleName, &vfs, headerSearchOpts,
      preprocessorOpts, clangCodeGenOpts, irGen.GetLLVMContext());

  clangCodeGen->Initialize(clangASTContext);
  return clangCodeGen;
}

IRGenModule::IRGenModule(IRGen &irGen, SourceFile *sourceFile,
                         llvm::StringRef moduleName,
                         llvm::StringRef outputFilename)

    : irGen(irGen), dataLayout(irGen.GetClangDataLayoutString()),
      triple(irGen.GetEffectiveClangTriple()),
      typeCache(irGen.GetLLVMContext()), outputFilename(outputFilename),
      clangCodeGen(CreateClangCodeGen(irGen, moduleName)), typeResolver(*this),
      metadata(*this) {

  // Setup module target
  irGen.AddIRGenModule(sourceFile, this);
}

/// Add an IRGenModule for a source file.
/// Should only be called from IRGenModule's constructor.
void IRGen::AddIRGenModule(SourceFile *sourceFile, IRGenModule *codeGenModule) {

  assert(irGenModules.count(sourceFile) == 0);
  irGenModules[sourceFile] = codeGenModule;
  if (!primaryCodeGenModule) {
    primaryCodeGenModule = codeGenModule;
  }
  queue.push_back(codeGenModule);
}

void IRGenModule::Setup() {
  GetClangCodeGen().GetModule()->setTargetTriple(GetTriple().str());
  GetClangCodeGen().GetModule()->setDataLayout(
      GetDataLayout().getStringRepresentation());
}

IRGenModule::~IRGenModule() {}

IRGenResult *
IRGenResult::Create(MemoryContext &memContext,
                    std::unique_ptr<llvm::LLVMContext> &&llvmContext,
                    std::unique_ptr<llvm::Module> &&llvmModule,
                    std::unique_ptr<llvm::TargetMachine> &&llvmTargetMachine) {

  // TODO: && may already take care of move
  return new (memContext)
      IRGenResult(std::move(llvmContext), std::move(llvmModule),
                  std::move(llvmTargetMachine));
}

llvm::StringRef IRGenModule::GetMangledName(Decl &d) {
  assert(false && "Not implemented!");
  return llvm::StringRef();
}

// llvm::GlobalValue *IRGenModule::GetGlobalValue(llvm::StringRef name) {
//   return GetCodeGenContext().GetLLVMModule()->getNamedValue(Name);
// }

llvm::Constant *
IRGenModule::CreateFunction(llvm::StringRef mangledName, FunctionDecl *fd,
                            llvm::Type *fnTy,
                            const EmitFunctionOptions emitFunctionOpts,
                            llvm::AttributeList extraAttrs) {

  llvm::GlobalValue *entry; //= GetGlobalValue(MangledName);

  auto isIncompleteFunction = false;
  llvm::FunctionType *llvmFunctionType = nullptr;

  if (llvm::isa<llvm::FunctionType>(fnTy)) {
    llvmFunctionType = llvm::cast<llvm::FunctionType>(fnTy);
  } else {
    llvmFunctionType =
        llvm::FunctionType::get(GetIRGenTypeCache().VoidTy, false);
    isIncompleteFunction = true;
  }

  llvm::Function *llvmFunction = llvm::Function::Create(
      llvmFunctionType, llvm::Function::ExternalLinkage,
      entry ? llvm::StringRef() : mangledName, GetClangCodeGen().GetModule());

  return llvmFunction;
}
llvm::Constant *
IRGenModule::GetOrCreateFunction(llvm::StringRef mangledName, FunctionDecl *fd,
                                 llvm::Type *fnTy,
                                 const EmitFunctionOptions emitFunctionOpts,
                                 llvm::AttributeList extraAttrs) {

  // TODO: Ignoring forDefinition for now -- just creating
  return CreateFunction(mangledName, fd, fnTy, emitFunctionOpts, extraAttrs);
}

llvm::Constant *
IRGenModule::GetFunctionAddress(FunctionDecl *fd, llvm::Type *fnTy,
                                const EmitFunctionOptions emitFunctionOpts) {

  // TODO:
  // If there was no specific requested type, just convert it now.
  // if (!functionTy) {
  //   const auto *FD = llvm::cast<FunctionDecl>(GD.getDecl());
  //   functionTy = GetIRGenTypeResolver().ResolveType(fd->GetType());
  // }

  if (emitFunctionOpts.contains(EmitFunctionFlags::IsForDefinition)) {
  }

  return GetOrCreateFunction("mangledName", fd, fnTy, emitFunctionOpts);
}

void IRGenModule::SetFunctionLinkage(FunctionDecl *fd, llvm::Function *fn) {
  fn->setLinkage(GetFunctionLinkage(fd));
}
llvm::GlobalValue::LinkageTypes
IRGenModule::GetFunctionLinkage(FunctionDecl *fd) {

  // TODO: FOR NOW
  return llvm::GlobalValue::InternalLinkage;
}

llvm::StringRef IRGenModule::GetMangledNameOfGlobalDecl(Decl *d) {
  const auto *nd = llvm::cast<NameableDecl>(d);
}

// TODO: Ok for now -- may move to IRGenMoulde
IRGenTypeCache::IRGenTypeCache(llvm::LLVMContext &llvmContext) {

  VoidTy = llvm::Type::getVoidTy(llvmContext);
  Int8Ty = llvm::Type::getInt8Ty(llvmContext);
  Int16Ty = llvm::Type::getInt16Ty(llvmContext);
  Int32Ty = llvm::Type::getInt32Ty(llvmContext);
  Int32PtrTy = Int32Ty->getPointerTo();
  Int64Ty = llvm::Type::getInt64Ty(llvmContext);
  Int8PtrTy = llvm::Type::getInt8PtrTy(llvmContext);

  // Int8PtrPtrTy = Int8PtrTy->getPointerTo(0);
}
