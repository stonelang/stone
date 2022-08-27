#include "stone/Compile/CompilerInstance.h"
#include "stone/Diag/CompilerDiagnostic.h"

using namespace stone;

CompilerInstance::CompilerInstance(CompilerInvocation &invocation)
    : invocation(invocation) {

  auto syntaxContext = std::make_unique<syn::SyntaxContext>(
      invocation.GetContext(), invocation.GetSearchPathOptions());
  syntax = std::make_unique<syn::Syntax>(std::move(syntaxContext));

  moduleSystem = std::make_unique<ModuleSystem>(
      *syntax.get(), invocation.GetContext(),
      invocation.GetCompilerOptions().moduleOpts);

  stats = std::make_unique<CompilerInstanceStats>(*this);
  invocation.GetContext().GetStatEngine().Register(stats.get());
}
CompilerInstance::~CompilerInstance() {}

std::unique_ptr<llvm::raw_fd_ostream>
CompilerInstance::GetFileOutputStream(llvm::StringRef outputFilename,
                                      Context &ctx) {
  std::error_code errCode;
  auto os = std::make_unique<llvm::raw_fd_ostream>(outputFilename, errCode,
                                                   llvm::sys::fs::OF_None);
  if (errCode) {
    ctx.GetDiagUnit().PrintD(SrcLoc(), diag::err_opening_output,
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

void CompilerInstanceStats::Print(ColorfulStream &stream) {
  // if (sc.GetCompilerOpts().printStats) {
  //   // GetContext().Out() << GetName() << '\n';
  //   return;
  // }
}
