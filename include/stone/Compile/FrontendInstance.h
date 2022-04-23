#ifndef STONE_COMPILE_LANG_H
#define STONE_COMPILE_LANG_H

#include "stone/Compile/FrontendInvocation.h"
#include "stone/Compile/FrontendUnit.h"
#include "stone/Compile/ModuleSystem.h"
#include "stone/Compile/PackageSystem.h"
#include "stone/Gen/CodeGenContext.h"
#include "stone/Sem/TypeCheckerListener.h"
#include "stone/Sem/TypeCheckerOptions.h"
#include "stone/Session/Mode.h"
#include "stone/Syntax/Module.h"
#include "stone/Syntax/Syntax.h"
#include "stone/Syntax/SyntaxContext.h"

namespace llvm {
class raw_pwrite_stream;
class GlobalVariable;
class MemoryBuffer;
class Module;
class TargetOptions;
class TargetMachine;
} // namespace llvm

namespace stone {

class FrontendInstance;
class FrontendListener;

using ModuleSyntaxFileUnion =
    llvm::PointerUnion<syn::Module *, syn::SyntaxFile *>;

using SyntaxAnalysisCallback = llvm::function_ref<void(syn::SyntaxFile &)>;
using SemanticAnalysisCallback = llvm::function_ref<void(FrontendInstance &)>;

using EachSyntaxFileCallback = llvm::function_ref<void(
    syn::SyntaxFile &, sem::TypeCheckerOptions &, TypeCheckerListener *)>;

class FrontendStats final : public Stats {
  FrontendInstance &lang;

public:
  FrontendStats(FrontendInstance &lang)
      : Stats("Frontend statistics:"), lang(lang) {}
  void Print(ColorfulStream &stream) override;
};

// TODO: SmallString<128> workingDirectory;
// llvm::sys::fs::current_path(workingDirectory);

// class FrontendBase : public Session {};

class FrontendInstance final {
  friend FrontendStats;

  FrontendInvocation langInvocation;
  FrontendListener *listener = nullptr;

  std::unique_ptr<FrontendStats> stats;
  std::unique_ptr<syn::Syntax> syntax;

  llvm::StringRef name;
  llvm::StringRef path;

  std::unique_ptr<ModuleSystem> moduleSystem;

  std::unique_ptr<PackageSystem> pkgSystem;

  // /// Contains buffer IDs for input source code files.
  // std::vector<unsigned> inputSourceBufferIDs;

  // /// Identifies the set of input buffers in the SourceManager that are
  // /// considered primaries.
  // llvm::SetVector<unsigned> primaryBufferIDs;

public:
  FrontendInstance(const FrontendInstance &) = delete;
  void operator=(const FrontendInstance &) = delete;
  FrontendInstance(FrontendInstance &&) = delete;
  void operator=(FrontendInstance &&) = delete;

  FrontendInstance(FrontendListener *listener = nullptr);
  ~FrontendInstance();

public:
  void Initialize();
  void Finish();
  FrontendInvocation &GetFrontendInvocation() { return langInvocation; }

public:
  syn::Syntax &GetSyntax() { return *syntax.get(); }
  ModuleSystem &GetModuleSystem() { return *moduleSystem.get(); }

  PackageSystem &GetPackageSystem() { return *pkgSystem.get(); }

  // llvm::StringRef CreateOutputFile(unsigned srcID);
  llvm::StringRef ComputeSourceOutputFile(unsigned srcID);

  FrontendListener *GetListener() { return listener; }
  void SetListener(FrontendListener *l) { listener = l; }

public:
  /// Print the lanuage help
  void PrintHelp();

  /// Print the language version
  void PrintVersion();

public:
  /// Perform code analysis and code generation
  void Compile(llvm::ArrayRef<FrontendUnit *> &sources);

private:
  void CompileWithSyntaxAnalysis(llvm::ArrayRef<FrontendUnit *> &sources);
  void CompileWithSyntaxAnalysis(llvm::ArrayRef<FrontendUnit *> &sources,
                                 SyntaxAnalysisCallback client);

  void CompileWithSemanticAnalysis(llvm::ArrayRef<FrontendUnit *> &sources);
  void CompileWithSemanticAnalysis(llvm::ArrayRef<FrontendUnit *> &sources,
                                   SemanticAnalysisCallback client);

  void ForEachSyntaxFile(EachSyntaxFileCallback client);

  void ResolveUsings();

public:
  //== Utils ==//
  static std::unique_ptr<llvm::raw_fd_ostream>
  GetFileOutputStream(llvm::StringRef outputFilename, Context &ctx);
};
} // namespace stone

#endif
