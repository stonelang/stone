#include "stone/Compile/CompilerInstance.h"
#include "stone/Basic/Mem.h"
#include "stone/Diag/CompilerDiagnostic.h"
#include "stone/Sem/ImportResolution.h"

#include "clang/Basic/FileManager.h"
#include "clang/Frontend/CompilerInvocation.h"
#include "clang/Frontend/TextDiagnosticPrinter.h"

using namespace stone;
using namespace stone::syn;

CompilerInstance::CompilerInstance() = default;
CompilerInstance::~CompilerInstance() = default;

void CompilerInstance::Initialize(const CompilerInvocation &inputInvocation) {
  invocation = inputInvocation;

  syntaxContext.reset(new SyntaxContext(invocation.GetLangContext(),
                                        invocation.GetSearchPathOptions(),
                                        invocation.GetClangContext()));

  compilerStats.reset(new CompilerInstanceStats(*this));

  invocation.GetLangContext().GetStats().Register(compilerStats);
}

std::unique_ptr<llvm::raw_fd_ostream>
CompilerInstance::GetFileOutputStream(llvm::StringRef outputFilename,
                                      LangContext &lc) {
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
CompilerInstance::GetPrimaryFileSpecificPathsForSyntaxFile(
    const syn::SyntaxFile &sf) const {
  return invocation.GetCompilerOptions()
      .GetInputsAndOutputs()
      .GetPrimaryFileSpecificPathsForPrimary(sf.GetFilename());
}

void CompilerInstance::ResolveImports() {
  // Resolve imports for all the source files.
  for (auto *moduleFile : GetModuleSystem().GetMainModule()->GetFiles()) {
    if (auto *syntaxFile = dyn_cast<syn::SyntaxFile>(moduleFile))
      sem::ResolveImports(*syntaxFile);
  }
}

void CompilerInstance::ForEachSyntaxFileToTypeCheck(
    EachSyntaxFileToTypeCheckCallback notify) {

  if (invocation.GetTypeCheckMode() == TypeCheckMode::WholeModule) {
    for (auto moduleFile : GetModuleSystem().GetMainModule()->GetFiles()) {
      auto *syntaxFile = dyn_cast<syn::SyntaxFile>(moduleFile);
      if (!syntaxFile) {
        continue;
      }
      if (notify(*syntaxFile, invocation.GetTypeCheckerOptions(),
                 invocation.GetListener().IsError())) {
        return Status::Error();
      }
    }
  } else {
    for (auto *syntaxFile :
         GetModuleSystem().GetMainModule()->GetPrimarySyntaxFiles()) {
      if (notify(*syntaxFile, invocation.GetTypeCheckerOptions(),
                 invocation.GetListener())
              .IsError()) {
        return Status::Error();
      }
    }
  }

  Status CompilerInstance::ForEachSyntaxFile(
      std::function<Status(SourceFile &)> notify) {
    for (auto moduleFile : GetModuleSystem().GetMainModule()->GetFiles()) {
      auto *syntaxFile = dyn_cast<SourceFile>(moduleFile);
      if (!syntaxFile) {
        continue;
      }
      if (notify(*syntaxFile).IsError()) {
        return Status::Error();
      }
    }
    return Status();
  }

  void *stone::AllocateInCompilerInstance(
      size_t bytes, const CompilerInstance &compiler,
      mem::AllocationArena arena, unsigned alignment) {
    return nullptr;
  }

  // CodeGenContext &CompilerInstance::GetCodeGenContext() { return *cgc; }

  void CompilerPrettyStackTrace::print(llvm::raw_ostream & os) const override {

    //   auto effective =
    //   invocation.GetCompilerOptions().effectiveCompilerVersion; if (effective
    //   != version::Version::GetCurrentCompilerVersion()) {
    //     os << "Compiling with effective version " << effective;
    //   } else {
    //     os << "Compiling with the current invocationuage version";
    //   }
    //   if (Invocation.GetCompilerOptions().allowModuleWithCompilerErrors) {
    //     os << " while allowing modules with compiler errors";
    //   }
    //   os << "\n";
  }
  void CompilerInstanceStats::Print(ColorStream & stream) {
    // if (sc.GetCompilerOpts().printStats) {
    //   // GetLangContext().Out() << GetName() << '\n';
    //   return;
    // }
  }
