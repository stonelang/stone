#ifndef STONE_COMPILE_LANG_H
#define STONE_COMPILE_LANG_H

#include "stone/Compile/LangInvocation.h"
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

class LangInstance;
class LangListener;

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
  void Compile(llvm::ArrayRef<SourceUnit *> sources);

  void PerformParseOnly();
  void PerformSemanticAnalysis();

public:
  void ForEachFileToTypeCheck();
};
} // namespace stone

#endif
