#include "stone/CodeGen/CodeGenContext.h"
#include "stone/AST/ASTContext.h"
#include "stone/Basic/CodeGenOptions.h"
#include "stone/CodeGen/CodeGenBackend.h"

#include "llvm/MC/TargetRegistry.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/TargetParser/SubtargetFeature.h"

using namespace stone;

CodeGenContext::CodeGenContext(const CodeGenOptions &codeGenOpts,
                               ASTContext &astContext)
    : codeGenOpts(codeGenOpts), astContext(astContext),
      llvmContext(new llvm::LLVMContext()),
      llvmTargetMachine(CreateTargetMachine()) {}

llvm::Triple CodeGenContext::GetEffectiveClangTriple() {
  return llvm::Triple(astContext.GetClangImporter()
                          .GetClangInstance()
                          .getTarget()
                          .getTargetOpts()
                          .Triple);
}

const llvm::StringRef CodeGenContext::GetClangDataLayoutString() {
  return astContext.GetClangImporter()
      .GetClangInstance()
      .getTarget()
      .getDataLayoutString();
}

std::unique_ptr<llvm::TargetMachine> CodeGenContext::CreateTargetMachine() {
  return stone::CreateTargetMachine(GetCodeGenOptions(), GetASTContext());
}

/// Add an CodeGenModule for a source file.
/// Should only be called from CodeGenModule's constructor.
void CodeGenContext::AddCodeGenModule(SourceFile *sourceFile,
                                      CodeGenModule *cgm) {}

/// Add an CodeGenModule to the queue
/// Should only be called from IRGenModule's constructor.
void CodeGenContext::QueueCodeGenModule(SourceFile *sourceFile,
                                        CodeGenModule *cgm) {}

/// Get an CodeGenModule for a declaration context.
/// Returns the CodeGenModule of the containing source file, or if this
/// cannot be determined, returns the primary CodeGenModule.
CodeGenModule *CodeGenContext::GetCodeGenModule() {}
