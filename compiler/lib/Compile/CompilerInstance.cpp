#include "stone/Compile/CompilerInstance.h"
#include "stone/Diag/CompilerDiagnostic.h"

#include "clang/Basic/FileManager.h"
#include "clang/Frontend/CompilerInvocation.h"
#include "clang/Frontend/TextDiagnosticPrinter.h"

using namespace stone;

CompilerInstance::CompilerInstance(CompilerInvocation &invocation)
    : invocation(invocation),
      sc(new syn::SyntaxContext(invocation.GetLangContext(),
                                invocation.GetSearchPathOptions(),
                                invocation.GetClangContext())),
      stats(new CompilerInstanceStats(*this)),
      ms(new ModuleSystem(invocation, GetSyntaxContext())) {

  invocation.GetLangContext().GetStatEngine().Register(stats.get());
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
    const {}

const PrimaryFileSpecificPaths &
CompilerInstance::GetPrimaryFileSpecificPathsForPrimary(
    llvm::StringRef fileName) const {}

const PrimaryFileSpecificPaths &
CompilerInstance::GetPrimaryFileSpecificPathsForAtMostOnePrimary() const {}

const PrimaryFileSpecificPaths &
CompilerInstance::GetPrimaryFileSpecificPathsForSyntaxFile(
    const syn::SyntaxFile &sf) const {}

void CompilerInstanceStats::Print(ColorfulStream &stream) {
  // if (sc.GetCompilerOpts().printStats) {
  //   // GetLangContext().Out() << GetName() << '\n';
  //   return;
  // }
}
