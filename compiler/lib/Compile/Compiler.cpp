#include "stone/Compile/Compiler.h"
#include "stone/Diag/FrontendDiagnostic.h"

using namespace stone;

Compiler::Compiler(Frontend &frontend) : frontend(frontend) {

  auto syntaxContext = std::make_unique<syn::SyntaxContext>(
      frontend.GetContext(), frontend.GetSearchPathOptions());
  syntax = std::make_unique<syn::Syntax>(std::move(syntaxContext));

  moduleSystem =
      std::make_unique<ModuleSystem>(*syntax.get(), frontend.GetContext(),
                                     frontend.GetFrontendOptions().moduleOpts);

  stats = std::make_unique<CompilerStats>(*this);
  frontend.GetContext().GetStatEngine().Register(stats.get());

}
Compiler::~Compiler() {}

std::unique_ptr<llvm::raw_fd_ostream>
Compiler::GetFileOutputStream(llvm::StringRef outputFilename, Context &ctx) {
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
// void Compiler::FinishTypeCheck() {
// }

// llvm::StringRef Compiler::ComputeSourceOutputFile(unsigned srcID) {
//   assert(false && "Not implemented");
//   return llvm::StringRef();
// }


void CompilerStats::Print(ColorfulStream &stream) {
  // if (sc.GetFrontendOpts().printStats) {
  //   // GetContext().Out() << GetName() << '\n';
  //   return;
  // }
}
