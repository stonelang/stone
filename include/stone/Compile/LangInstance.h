#ifndef STONE_COMPILE_LANG_H
#define STONE_COMPILE_LANG_H

#include "stone/Compile/LangInvocation.h"
#include "stone/Compile/ModuleSystem.h"
#include "stone/Compile/PackageSystem.h"
#include "stone/Compile/SourceUnit.h"
#include "stone/Compile/TypeCheckerListener.h"
#include "stone/Compile/TypeCheckerOptions.h"

#include "stone/Gen/CodeGenContext.h"
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

class LangInstance;
class LangListener;

using ModuleSyntaxFileUnion =
    llvm::PointerUnion<syn::Module *, syn::SyntaxFile *>;

using SyntaxAnalysisCallback = llvm::function_ref<void(syn::SyntaxFile &)>;
using SemanticAnalysisCallback = llvm::function_ref<void(LangInstance &)>;

using EachSyntaxFileCallback = llvm::function_ref<void(
    syn::SyntaxFile &, types::TypeCheckerOptions &, TypeCheckerListener *)>;

class LangStats final : public Stats {
  LangInstance &lang;

public:
  LangStats(LangInstance &lang) : Stats("Lang statistics:"), lang(lang) {}
  void Print(ColorfulStream &stream) override;
};

// TODO: SmallString<128> workingDirectory;
// llvm::sys::fs::current_path(workingDirectory);

class LangInstance final {
  friend LangStats;

  LangInvocation langInvocation;
  LangListener *listener = nullptr;

  std::unique_ptr<LangStats> stats;
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
  LangInstance(const LangInstance &) = delete;
  void operator=(const LangInstance &) = delete;
  LangInstance(LangInstance &&) = delete;
  void operator=(LangInstance &&) = delete;

  LangInstance(LangListener *listener = nullptr);
  ~LangInstance();

public:
  void Initialize();
  void Finish();
  LangInvocation &GetLangInvocation() { return langInvocation; }

public:
  syn::Syntax &GetSyntax() { return *syntax.get(); }
  ModuleSystem &GetModuleSystem() { return *moduleSystem.get(); }

  PackageSystem &GetPackageSystem() { return *pkgSystem.get(); }

  // llvm::StringRef CreateOutputFile(unsigned srcID);
  llvm::StringRef ComputeSourceOutputFile(unsigned srcID);

  LangListener *GetListener() { return listener; }
  void SetListener(LangListener *l) { listener = l; }

public:
  /// Print the lanuage help
  void PrintHelp();

  /// Print the language version
  void PrintVersion();

public:
  /// Perform code analysis and code generation
  void Compile(llvm::ArrayRef<SourceUnit *> &sources);

private:
  void CompileWithSyntaxAnalysis(llvm::ArrayRef<SourceUnit *> &sources);
  void CompileWithSyntaxAnalysis(llvm::ArrayRef<SourceUnit *> &sources,
                                 SyntaxAnalysisCallback client);

  void CompileWithSemanticAnalysis(llvm::ArrayRef<SourceUnit *> &sources);
  void CompileWithSemanticAnalysis(llvm::ArrayRef<SourceUnit *> &sources,
                                   SemanticAnalysisCallback client);

  void ForEachSyntaxFile(EachSyntaxFileCallback client);

  void ResolveUsings();

public:
  //= Utils =//
  static std::unique_ptr<llvm::raw_fd_ostream>
  GetFileOutputStream(llvm::StringRef outputFilename, Context &ctx);
};
} // namespace stone

#endif
