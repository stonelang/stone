#include "stone/Compile/CompilerInstance.h"
#include "stone/Diag/CompilerDiagnostic.h"
#include "stone/Sem/ImportResolution.h"

#include "clang/Basic/FileManager.h"
#include "clang/Frontend/CompilerInvocation.h"
#include "clang/Frontend/TextDiagnosticPrinter.h"

using namespace stone;

CompilerInstance::CompilerInstance(CompilerInvocation &invocation)
    : invocation(invocation),
      sc(new ast::ASTContext(invocation.GetLangContext(),
                             invocation.GetSearchPathOptions(),
                             invocation.GetClangContext())),
      stats(new CompilerInstanceStats(*this)),
      ms(new ModuleSystem(invocation, GetASTContext())) {

  invocation.GetLangContext().GetStatEngine().Register(stats.get());

  // CreateCodeGenContext();
}
CompilerInstance::~CompilerInstance() {}

std::unique_ptr<llvm::raw_fd_ostream>
CompilerInstance::GetFileOutputStream(llvm::StringRef outputFilename,
                                      LangContext &lc) {
  std::error_code errCode;
  auto os = std::make_unique<llvm::raw_fd_ostream>(outputFilename, errCode,
                                                   llvm::sys::fs::OF_None);
  if (errCode) {
    lc.GetDiagUnit().PrintD(SrcLoc(), diag::err_opening_output,
                            diag::LLVMStr(outputFilename),
                            diag::LLVMStr(errCode.message()));
    return nullptr;
  }
  return os;
}

// void CompilerInstance::FinishTypeCheck() {
// }

// llvm::StringRef CompilerInstance::ComputeSourceOutputFile(unsigned srcID) {
//   assert(false && "Not implemented");
//   return llvm::StringRef();
// }

const PrimaryFileSpecificPaths &
CompilerInstance::GetPrimaryFileSpecificPathsForWholeModuleOptimizationMode()
    const {
  return GetPrimaryFileSpecificPathsForAtMostOnePrimary();
}
const PrimaryFileSpecificPaths &
CompilerInstance::GetPrimaryFileSpecificPathsForAtMostOnePrimary() const {
  return invocation.GetCompilerOptions()
      .GetInputsAndOutputs()
      .GetPrimaryFileSpecificPathsForAtMostOnePrimary();
}
const PrimaryFileSpecificPaths &
CompilerInstance::GetPrimaryFileSpecificPathsForPrimary(
    StringRef filename) const {
  return invocation.GetCompilerOptions()
      .GetInputsAndOutputs()
      .GetPrimaryFileSpecificPathsForPrimary(filename);
}
const PrimaryFileSpecificPaths &
CompilerInstance::GetPrimaryFileSpecificPathsForASTFile(
    const ast::ASTFile &sf) const {
  return invocation.GetCompilerOptions()
      .GetInputsAndOutputs()
      .GetPrimaryFileSpecificPathsForPrimary(sf.GetFilename());
}

void CompilerInstance::ResolveImports() {
  // Resolve imports for all the source files.
  for (auto *moduleFile : GetModuleSystem().GetMainModule()->GetFiles()) {
    if (auto *asttaxFile = dyn_cast<ast::ASTFile>(moduleFile))
      sem::ResolveImports(*asttaxFile);
  }
}

void CompilerInstance::ForEachASTFile(EachASTFileCallback client) {

  switch (invocation.GetTypeCheckMode()) {
  case TypeCheckMode::WholeModule: {
    for (auto moduleFile : GetModuleSystem().GetMainModule()->GetFiles()) {
      auto *asttaxFile = dyn_cast<ast::ASTFile>(moduleFile);
      if (asttaxFile) {
        client(*asttaxFile, invocation.GetTypeCheckerOptions(),
               invocation.GetListener());
      }
    }
    break;
  }
  case TypeCheckMode::EachFile: {
    for (auto *asttaxFile :
         GetModuleSystem().GetMainModule()->GetPrimaryASTFiles()) {
      client(*asttaxFile, invocation.GetTypeCheckerOptions(),
             invocation.GetListener());
    }
    break;
  }
  default: {
  }
  }
}

// CodeGenContext &CompilerInstance::GetCodeGenContext() { return *cgc; }

void CompilerInstanceStats::Print(ColorStream &stream) {
  // if (sc.GetCompilerOpts().printStats) {
  //   // GetLangContext().Out() << GetName() << '\n';
  //   return;
  // }
}
