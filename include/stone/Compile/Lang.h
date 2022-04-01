#ifndef STONE_COMPILE_LANG_H
#define STONE_COMPILE_LANG_H

#include "stone/Compile/Frontend.h"
#include "stone/Compile/ModuleSystem.h"
#include "stone/Compile/SourceUnit.h"
#include "stone/Gen/CodeGenContext.h"
#include "stone/Session/Mode.h"
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
  LangStats(Lang &lang)
      : Stats("Lang statistics:"), lang(lang) {}
  void Print(ColorfulStream& stream) override;
};

// TODO: SmallString<128> workingDirectory;
// llvm::sys::fs::current_path(workingDirectory);

class Lang final {
  friend LangStats;

  Frontend frontend;
  LangListener *listener = nullptr;

  std::unique_ptr<LangStats> stats;
  std::unique_ptr<syn::Syntax> syntax;

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
  Frontend &GetFrontend() { return frontend; }

public:
  syn::Syntax &GetSyntax() { return *syntax.get(); }

  ModuleSystem &GetModuleSystem() { return *moduleSystem.get(); }

  // llvm::StringRef CreateOutputFile(unsigned srcID);
  llvm::StringRef ComputeSourceOutputFile(unsigned srcID);

  /// TODO: Something to think about -- right now this is in Context.
  // void AddDiagnosticListener(DiagnosticListener* listener);

  LangListener *GetListener() { return listener; }
  void SetListener(LangListener *l) { listener = l; }

public:
  /// Perform code analysis and code generation
  void Compile(llvm::ArrayRef<SourceUnit *> sources);

  // Perform code analysis
  void PerformAnalysis(llvm::ArrayRef<SourceUnit *> sources);

  /// Perform code analysis on source code.
  void PerformAnalysis(SourceUnit &source);

  /// Print the lanuage help
  void PrintHelp();

  /// Print the language version
  void PrintVersion();

private:
  /// Parse a single file and return a syntax tree
  syn::SyntaxFile *Parse(unsigned srcID);

  void ResolveUse();

  /// Print out the syntax tree
  void EmitParse(syn::SyntaxFile *sf);

  /// Perform type-checking on the SyntaxFile
  void TypeCheckSyntaxFile(syn::SyntaxFile &sf);

  /// Perform type-checking on the entire module
  void TypeCheckModule(syn::Module *mod);

  /// Emit the syntax after performing type-checking
  void EmitSyntax(syn::SyntaxFile *sf);

private:
  // Peform code generation
  void PerformCodeGen();

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
