#include "stone/Compile/Compiler.h"
#include "stone/Basic/Mem.h"
#include "stone/Diag/CompilerDiagnostic.h"
#include "stone/Public.h"

#include "clang/Basic/FileManager.h"
#include "clang/Frontend/CompilerInvocation.h"
#include "clang/Frontend/TextDiagnosticPrinter.h"

using namespace stone;
using namespace stone::syn;

// Compiler::Compiler() : queue(*this), stats(new CompilerStats(*this)) {
//   GetCompilerContext().GetLangContext().GetStats().Register(stats.get());
// }

Compiler::Compiler(const CompilerIvocation &invocation)
    : invocation(invocation), compilerQueue(new CompilerQueue(*this)),
      compilerStats(new CompilerStats(*this)) {}

Compiler::~Compiler() = default;

Status Compiler::Initialize() {

  assert(invocation.GetAction().CanCompile());
  SetupASTContext();
  SetupSources();
  SetupModules();
  SetupCodeGenConext();
  return Status();
}
/// Setup ASTContext based on the action

void Compiler::SetupASTContext() {
  syntaxContext.reset(new ASTContext(
      invocation.GetLangContext(), invocation.GetSearchPathOptions(),
      GetClangContext(), invocation.GetDiags(), GetStats()));
}

void Compiler::SetupCodeGenContext() {

  if (invocation.GetAction().CanCodeGen()) {
  }

  // syntaxContext.reset(new ASTContext(
  //     invocation.GetLangContext(), invocation.GetSearchPathOptions(),
  //     GetClangContext(), invocation.GetDiags(), GetStats()));
}

void Compiler::SetupOutputBackend() {}

// std::unique_ptr<llvm::raw_fd_ostream>
// Compiler::GetFileOutputStream(llvm::StringRef outputFilename, LangContext
// &lc) {
//   std::error_code errCode;
//   auto os = std::make_unique<llvm::raw_fd_ostream>(outputFilename, errCode,
//                                                    llvm::sys::fs::OF_None);
//   if (errCode) {
//     lc.GetDiags().PrintD(SrcLoc(), diag::err_opening_output,
//                          diag::LLVMStr(outputFilename),
//                          diag::LLVMStr(errCode.message()));
//     return nullptr;
//   }
//   return os;
// }

// void Compiler::FinishTypeCheck() {
// }

// llvm::StringRef Compiler::ComputeSourceOutputFile(unsigned srcID) {
//   assert(false && "Not implemented");
//   return llvm::StringRef();
// }

const PrimaryFileSpecificPaths &
Compiler::GetPrimaryFileSpecificPathsForWholeModuleOptimizationMode() const {
  return GetPrimaryFileSpecificPathsForAtMostOnePrimary();
}
const PrimaryFileSpecificPaths &
Compiler::GetPrimaryFileSpecificPathsForAtMostOnePrimary() const {
  return GetCompilerOptions()
      .GetInputsAndOutputs()
      .GetPrimaryFileSpecificPathsForAtMostOnePrimary();
}
const PrimaryFileSpecificPaths &
Compiler::GetPrimaryFileSpecificPathsForPrimary(StringRef filename) const {
  return GetCompilerOptions()
      .GetInputsAndOutputs()
      .GetPrimaryFileSpecificPathsForPrimary(filename);
}
const PrimaryFileSpecificPaths &
Compiler::GetPrimaryFileSpecificPathsForASTFile(const syn::ASTFile &sf) const {
  return GetCompilerOptions()
      .GetInputsAndOutputs()
      .GetPrimaryFileSpecificPathsForPrimary(sf.GetFilename());
}

// void Compiler::ResolveImports() {
//   // Resolve imports for all the source files.
//   for (auto *moduleFile : GetModuleSystem().GetMainModule()->GetFiles()) {
//     if (auto *astFile = dyn_cast<syn::ASTFile>(moduleFile))
//       stone::ResolveASTFileImports(*astFile);
//   }
// }

// Status Compiler::ForEachASTFileToTypeCheck(
//     EachASTFileToTypeCheckCallback notify) {

// if (GetCompilerContext().GetTypeCheckMode() == TypeCheckMode::WholeModule) {

//   for (auto moduleFile : GetModuleSystem().GetMainModule()->GetFiles()) {
//     auto *astFile = dyn_cast<syn::ASTFile>(moduleFile);
//     if (!astFile) {
//       continue;
//     }
//     if (notify(*astFile, GetCompilerContext().GetTypeCheckerOptions(),
//                GetListener())
//             .IsError()) {
//       return Status::Error();
//     }
//   }
// } else {
//   for (auto *astFile :
//        GetModuleSystem().GetMainModule()->GetPrimaryASTFiles()) {
//     if (notify(*astFile, GetCompilerContext().GetTypeCheckerOptions(),
//                GetListener())
//             .IsError()) {
//       return Status::Error();
//     }
//   }
// }

//   return Status();
// }

// Status Compiler::ForEachASTFile(EachASTFileCallback notify) {

// for (auto moduleFile : GetModuleSystem().GetMainModule()->GetFiles()) {
//   auto *astFile = dyn_cast<ASTFile>(moduleFile);
//   if (!astFile) {
//     continue;
//   }
//   if (notify(*astFile).IsError()) {
//     return Status::Error();
//   }
// }
// return Status();
//}

void Compiler::AddDiagnosticConsumer(DiagnosticConsumer &consumer) {
  diags.AddConsumer(consumer);
}

size_t Compiler::GetTotalMemUsed() const { return bumpAlloc.getTotalMemory(); }

// void Compiler::PrintTimers() {}
// void Compiler::PrintDiagnostics() {}
// void Compiler::PrintStatistics() {}

void *stone::AllocateInCompiler(size_t bytes, const Compiler &compiler,
                                mem::AllocationArena arena,
                                unsigned alignment) {
  return compiler.Allocate(bytes, alignment /*, arena*/);
}

void Compiler::Finalize() {

  // PrintDiagnostics();
  // PrintStatistics();
}

void Compiler::PrintDiagnostics() { GetDiags().Finish(); }
void Compiler::PrintStatistics() {}

void CompilerPrettyStackTrace::print(llvm::raw_ostream &os) const {

  //   auto effective =
  //   GetCompilerContext().GetCompilerOptions().effectiveCompilerVersion; if
  //   (effective
  //   != version::Version::GetCurrentCompilerVersion()) {
  //     os << "Compiling with effective version " << effective;
  //   } else {
  //     os << "Compiling with the current compilerInvocationuage version";
  //   }
  //   if
  //   (Invocation.GetCompilerContext().GetCompilerOptions().allowModuleWithCompilerErrors)
  //   {
  //     os << " while allowing modules with compiler errors";
  //   }
  //   os << "\n";
}
void CompilerStats::Print(ColorStream &stream) {
  // if (sc.GetCompilerOpts().printStats) {
  //   // GetLangContext().Out() << GetName() << '\n';
  //   return;
  // }
}
