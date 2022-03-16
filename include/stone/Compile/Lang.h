#ifndef STONE_COMPILE_LANG_H
#define STONE_COMPILE_LANG_H

#include "stone/Compile/LangContext.h"
#include "stone/Compile/ModuleSystem.h"
#include "stone/Compile/SourceUnit.h"
#include "stone/Gen/CodeGenContext.h"
#include "stone/Option/Mode.h"
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

class Lang;
class LangListener;

class LangStats final : public Stats {
  Lang &lang;

public:
  LangStats(Lang &lang, Context &ctx)
      : Stats("Lang statistics:", ctx), lang(lang) {}
  void Print() override;
};

// TODO: SmallString<128> workingDirectory;
// llvm::sys::fs::current_path(workingDirectory);

class Lang final {

  friend LangStats;

  LangContext lc;
  LangListener *listener = nullptr;

  std::unique_ptr<LangStats> stats;
  std::unique_ptr<syn::Syntax> syntax;
  std::unique_ptr<syn::SyntaxContext> tc;

  bool isEoc = false;

  llvm::StringRef name;
  llvm::StringRef path;

  std::unique_ptr<ModuleSystem> moduleSystem;

public:
  Lang(const Lang &) = delete;
  void operator=(const Lang &) = delete;
  Lang(Lang &&) = delete;
  void operator=(Lang &&) = delete;

  Lang(LangListener *listener = nullptr);
  ~Lang();

public:
  void Initialize();
  void Finish();
  LangContext &GetLangContext() { return lc; }

public:
  syn::SyntaxContext &GetSyntaxContext() { return *tc.get(); }
  syn::Syntax &GetSyntax() { return *syntax.get(); }

  ModuleSystem &GetModuleSystem() { return *moduleSystem.get(); }

  // TODO:
  void SetModuleName(llvm::StringRef name) {
    lc.GetLangOptions().systemOpts.moduleName = name.data();
  }
  const llvm::StringRef GetModuleName() const {
    return lc.GetLangOptions().systemOpts.moduleName;
  }

  bool IsEoc() { return isEoc; }
  void Stopc() { isEoc = true; }

  // llvm::StringRef CreateOutputFile(unsigned srcID);
  llvm::StringRef ComputeSourceOutputFile(unsigned srcID);

  /// TODO: Something to think about -- right now this is in Context.
  // void AddDiagnosticListener(DiagnosticListener* listener);

  LangListener *GetListener() { return listener; }
  void SetListener(LangListener *l) { listener = l; }

public:
  /// Perform code analysis and code generation
  void Compile();

  // Perform code analysis
  void PerformCodeAnalysis();

  /// Print the lanuage help
  void PrintHelp();

  /// Print the language version
  void PrintVersion();

private:
  /// Perform code analysis on source code.
  void PerformCodeAnalysis(const unsigned srcID);

  /// Parse a single file and return a syntax tree
  syn::SyntaxFile *Parse(unsigned srcID);

  /// Print out the syntax tree
  void EmitParse(syn::SyntaxFile *sf);

  /// Perform type-checking on the SyntaxFile
  void TypeCheckSyntaxFile(syn::SyntaxFile &sf);

  /// Perform type-checking on the entire module
  void TypeCheckModule(syn::Module *mod);

  /// Emit the syntax after performing type-checking
  void EmitSyntax(syn::SyntaxFile *sf);

  bool JustCodeAnalysis() {
    if (lc.GetMode().JustParse() || lc.GetMode().JustTypeCheck()) {
      return true;
    }
    return false;
  }

private:
  // Peform code generation
  void PerformCodeGen();

  bool CanCodeGen() {
    switch (lc.GetMode().GetKind()) {
    case ModeKind::None:
    case ModeKind::EmitIR:
    case ModeKind::EmitBC:
    case ModeKind::EmitObject:
    case ModeKind::EmitAssembly:
    case ModeKind::EmitModule:
    case ModeKind::EmitLibrary:
      return true;
    default:
      return false;
    }
  }
  /// Generate the IR for an entire module
  llvm::Module *GenIR(syn::Module &sf, CodeGenContext &cc);

  /// Generate IR a single SyntaxFile
  llvm::Module *GenIR(syn::SyntaxFile &sf, CodeGenContext &cc);

  /// Perform optimzation on the SyntaxFile
  void OptimizeIR(llvm::Module *mod);

  /// Generate Object file
  void GenObject(unsigned srcID, llvm::Module *mod, CodeGenContext &cc);

  /// Generate Object file
  void GenBitCode();

  /// Generates a 'test.stonem' file
  void GenModule();
};

} // namespace stone

#endif
