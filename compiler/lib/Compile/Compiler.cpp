#include "stone/Compile/Compiler.h"
#include "stone/Basic/Mem.h"
#include "stone/Diag/CompilerDiagnostic.h"
#include "stone/Sem/ImportResolution.h"

#include "clang/Basic/FileManager.h"
#include "clang/Frontend/CompilerInvocation.h"
#include "clang/Frontend/TextDiagnosticPrinter.h"

using namespace stone;
using namespace stone::syn;

Compiler::Compiler() : queue(*this), stats(new CompilerStats(*this)) {
  GetConfig().GetLangContext().GetStats().Register(stats.get());
}
Compiler::~Compiler() = default;

void Compiler::Setup() {

  // compilerStats.reset(new CompilerStats(*this));
  // GetConfig().GetLangContext().GetStats().Register(compilerStats);
  // SetupSyntaxContext();
}

/// Setup SyntaxContext based on the action
void Compiler::SetupSyntaxContext() {

  syntaxContext.reset(new SyntaxContext(GetConfig().GetLangContext(),
                                        GetConfig().GetSearchPathOptions(),
                                        GetConfig().GetClangContext()));
}

std::unique_ptr<llvm::raw_fd_ostream>
Compiler::GetFileOutputStream(llvm::StringRef outputFilename, LangContext &lc) {
  std::error_code errCode;
  auto os = std::make_unique<llvm::raw_fd_ostream>(outputFilename, errCode,
                                                   llvm::sys::fs::OF_None);
  if (errCode) {
    lc.GetDiags().PrintD(SrcLoc(), diag::err_opening_output,
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

const PrimaryFileSpecificPaths &
Compiler::GetPrimaryFileSpecificPathsForWholeModuleOptimizationMode() const {
  return GetPrimaryFileSpecificPathsForAtMostOnePrimary();
}
const PrimaryFileSpecificPaths &
Compiler::GetPrimaryFileSpecificPathsForAtMostOnePrimary() const {
  return GetConfig()
      .GetCompilerOptions()
      .GetInputsAndOutputs()
      .GetPrimaryFileSpecificPathsForAtMostOnePrimary();
}
const PrimaryFileSpecificPaths &
Compiler::GetPrimaryFileSpecificPathsForPrimary(StringRef filename) const {
  return GetConfig()
      .GetCompilerOptions()
      .GetInputsAndOutputs()
      .GetPrimaryFileSpecificPathsForPrimary(filename);
}
const PrimaryFileSpecificPaths &
Compiler::GetPrimaryFileSpecificPathsForSyntaxFile(
    const syn::SyntaxFile &sf) const {
  return GetConfig()
      .GetCompilerOptions()
      .GetInputsAndOutputs()
      .GetPrimaryFileSpecificPathsForPrimary(sf.GetFilename());
}

void Compiler::ResolveImports() {
  // Resolve imports for all the source files.
  for (auto *moduleFile : GetModuleSystem().GetMainModule()->GetFiles()) {
    if (auto *syntaxFile = dyn_cast<syn::SyntaxFile>(moduleFile))
      sem::ResolveImports(*syntaxFile);
  }
}

Status Compiler::ForEachSyntaxFileToTypeCheck(
    EachSyntaxFileToTypeCheckCallback notify) {

  // if (GetConfig().GetTypeCheckMode() == TypeCheckMode::WholeModule) {

  //   for (auto moduleFile : GetModuleSystem().GetMainModule()->GetFiles()) {
  //     auto *syntaxFile = dyn_cast<syn::SyntaxFile>(moduleFile);
  //     if (!syntaxFile) {
  //       continue;
  //     }
  //     if (notify(*syntaxFile, GetConfig().GetTypeCheckerOptions(),
  //                GetListener())
  //             .IsError()) {
  //       return Status::Error();
  //     }
  //   }
  // } else {
  //   for (auto *syntaxFile :
  //        GetModuleSystem().GetMainModule()->GetPrimarySyntaxFiles()) {
  //     if (notify(*syntaxFile, GetConfig().GetTypeCheckerOptions(),
  //                GetListener())
  //             .IsError()) {
  //       return Status::Error();
  //     }
  //   }
  // }

  return Status();
}

Status Compiler::ForEachSyntaxFile(EachSyntaxFileCallback notify) {

  // for (auto moduleFile : GetModuleSystem().GetMainModule()->GetFiles()) {
  //   auto *syntaxFile = dyn_cast<SyntaxFile>(moduleFile);
  //   if (!syntaxFile) {
  //     continue;
  //   }
  //   if (notify(*syntaxFile).IsError()) {
  //     return Status::Error();
  //   }
  // }
  return Status();
}

void Compiler::SetupDiagnostics(DiagnosticListener &listener) {
  GetDiags().AddListener(listener);
}

size_t Compiler::GetTotalMemUsed() const { return bumpAlloc.getTotalMemory(); }

void Compiler::PrintTimers() {}
void Compiler::PrintDiagnostics() {}
void Compiler::PrintStatistics() {}

void *stone::AllocateInCompiler(size_t bytes, const Compiler &compiler,
                                mem::AllocationArena arena,
                                unsigned alignment) {
  return nullptr;
}

void Compiler::Finish() {}
CompilerAction::CompilerAction() {}
// CodeGenContext &Compiler::GetCodeGenContext() { return *cgc; }

void CompilerPrettyStackTrace::print(llvm::raw_ostream &os) const {

  //   auto effective =
  //   GetConfig().GetCompilerOptions().effectiveCompilerVersion; if
  //   (effective
  //   != version::Version::GetCurrentCompilerVersion()) {
  //     os << "Compiling with effective version " << effective;
  //   } else {
  //     os << "Compiling with the current compilerInvocationuage version";
  //   }
  //   if
  //   (Invocation.GetConfig().GetCompilerOptions().allowModuleWithCompilerErrors)
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
