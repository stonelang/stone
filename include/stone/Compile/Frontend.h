#ifndef STONE_COMPILE_FRONTEND_H
#define STONE_COMPILE_FRONTEND_H

#include "stone/Basic/CodeGenOptions.h"
#include "stone/Basic/FileSystemOptions.h"
#include "stone/Basic/Mem.h"
#include "stone/Basic/ModuleOptions.h"
#include "stone/Basic/SrcLoc.h"
#include "stone/Compile/FrontendOptions.h"
#include "stone/Compile/FrontendUnit.h"
#include "stone/Compile/ModuleSystem.h"
#include "stone/Compile/PackageSystem.h"
#include "stone/Context.h"
#include "stone/Gen/CodeGenContext.h"
#include "stone/Sem/TypeCheckerListener.h"
#include "stone/Sem/TypeCheckerOptions.h"
#include "stone/Session/Mode.h"
#include "stone/Session/Session.h"
#include "stone/Syntax/Module.h"
#include "stone/Syntax/SearchPathOptions.h"
#include "stone/Syntax/Syntax.h"
#include "stone/Syntax/SyntaxContext.h"

#include "llvm/ADT/SetVector.h"
#include "llvm/Option/ArgList.h"

namespace llvm {
class raw_pwrite_stream;
class GlobalVariable;
class MemoryBuffer;
class Module;
class TargetOptions;
class TargetMachine;
} // namespace llvm

namespace stone {

class Frontend;
class FrontendListener;

using ModuleSyntaxFileUnion =
    llvm::PointerUnion<syn::Module *, syn::SyntaxFile *>;

using SyntaxAnalysisCallback = llvm::function_ref<void(syn::SyntaxFile &)>;
using SemanticAnalysisCallback = llvm::function_ref<void(Frontend &)>;

using EachSyntaxFileCallback = llvm::function_ref<void(
    syn::SyntaxFile &, sem::TypeCheckerOptions &, TypeCheckerListener *)>;

// using CompileWithGenIRCallback = llvm::function_ref<void(
//     Frontend &frontend, CodeGenContext &cgc, IRCodeGenResult &result)>;

class FrontendStats final : public Stats {
  Frontend &frontend;

public:
  FrontendStats(Frontend &frontend)
      : Stats("Frontend statistics:"), frontend(frontend) {}
  void Print(ColorfulStream &stream) override;
};

class FrontendBase : public Session {
protected:
  std::unique_ptr<FrontendOptions> frontendOpts;

  /// Options for generating code
  CodeGenOptions codeGenOpts;

  /// The options for searching libs
  SearchPathOptions searchPathOpts;

  /// The options for type-checking
  TypeCheckerOptions typeCheckerOpts;

  TargetOptions targetOpts;

  /// The main executable path of the running program
  std::string mainExecutablePath;

  /// Contains buffer IDs for input source code files.
  std::vector<unsigned> sourceBufferIDs;

  // The primary Sources
  llvm::SetVector<unsigned> primarySourceBufferIDs;

  /// Allocator FrontendUnit
  mutable llvm::BumpPtrAllocator bumpAlloc;

  llvm::MemoryBuffer *codeCompletionBuffer = nullptr;
  /// Code completion offset in bytes from the beginning of the main
  /// source file.  Valid only if \c isCodeCompletion() == true.
  unsigned codeCompletionOffset = ~0U;

public:
  FrontendBase(llvm::StringRef programName, llvm::StringRef programPath);
  ~FrontendBase();

public:
  // llvm::ArrayRef<FrontendUnit *> BuildSources(const file::Files &inputs);
  // FrontendUnit *BuildSource(const file::File &input);

  Error CreateSourceBuffers();
  unsigned CreateSourceBuffer(const FrontendInputFile &input);

  /// Return whether there is an entry in PrimaryInputs for buffer \p BufID.
  bool IsPrimarySourceID(unsigned primarySourceID) const {
    return primarySourceIDs.count(primarySourceID) != 0;
  }
  void RecordPrimarySourceID(unsigned primarySourceID);

  llvm::Optional<unsigned> CreateCodeCompletionBuffer();

  /// Gets the set of SourceFiles which are the primary inputs for this
  /// CompilerInstance.
  // llvm::ArrayRef<syn::SyntaxFile *> GetPrimaryFiles() const {
  //   return GetModuleSystem().GetMainModule()->GetPrimaryFiles();
  // }

  stone::Error ComputeOptions(llvm::opt::InputArgList &args) override;
  std::unique_ptr<OutputFile> ComputeOutputFile(FrontendUnit &source);

  // TODO: update FrontendOptions
  void ComputeModuleOutputMode() { assert(false && "Not implemented"); }

public:
  FrontendOptions &GetFrontendOptions() { return *frontendOpts; }
  const FrontendOptions &GetFrontendOptions() const { return *frontendOpts; }

  CodeGenOptions &GetCodeGenOptions() { return codeGenOpts; }
  const CodeGenOptions &GetCodeGenOptions() const { return codeGenOpts; }

  TargetOptions &GetTargetOptions() { return targetOpts; }
  const TargetOptions &GetTargetOptions() const { return targetOpts; }

  TypeCheckerOptions &GetTypeCheckerOptions() { return typeCheckerOpts; }
  const TypeCheckerOptions &GetTypeCheckerOptions() const {
    return typeCheckerOpts;
  }

  TypeCheckMode GetTypeCheckMode() {
    return (primarySourceIDs.empty() ? TypeCheckMode::WholeModule
                                     : TypeCheckMode::EachFile);
    // TODO: Set in ParseArgs return GetTypeCheckerOptions().typeCheckMode;
  }

  bool HasError() { return GetContext().GetDiagUnit().HasError(); }

  bool JustFrontend() {
    if (GetFrontendOptions().GetMode().JustParse() ||
        GetFrontendOptions().GetMode().JustTypeCheck() ||
        GetFrontendOptions().GetMode().IsEmitIR()) {
      return true;
    }
    return false;
  }
};

class Frontend final : public FrontendBase {
  friend FrontendStats;

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
  Frontend(const Frontend &) = delete;
  void operator=(const Frontend &) = delete;
  Frontend(Frontend &&) = delete;
  void operator=(Frontend &&) = delete;

  Frontend(llvm::StringRef programName, llvm::StringRef programPath,
           FrontendListener *listener = nullptr);
  ~Frontend();

public:
  void Finish();

public:
  syn::Syntax &GetSyntax() { return *syntax.get(); }
  ModuleSystem &GetModuleSystem() { return *moduleSystem.get(); }
  PackageSystem &GetPackageSystem() { return *pkgSystem.get(); }

  // llvm::StringRef CreateOutputFile(unsigned srcID);
  // llvm::StringRef ComputeSourceOutputFile(unsigned srcID);

  FrontendListener *GetListener() { return listener; }
  void SetListener(FrontendListener *l) { listener = l; }

public:
  /// Perform code analysis and code generation
  void Compile();

private:
  void CompileWithSyntaxAnalysis();
  void CompileWithSyntaxAnalysis(SyntaxAnalysisCallback client);

  void CompileWithSemanticAnalysis();
  void CompileWithSemanticAnalysis(SemanticAnalysisCallback client);

  // void CompileWithGenIR(stone::ModuleSyntaxFileUnion msf, CodeGenContext
  // &cgc,
  //                       CompileWithGenIRCallback client);

  void ForEachSyntaxFile(EachSyntaxFileCallback client);

  void ResolveUsings();

public:
  ModuleOutputMode GetModuleOutputMode() {
    // TODO: This must be computed in the future.
    return GetModuleSystem().GetModuleOptions().moduleOutputMode;
  }

  void PrintHelp(const llvm::opt::OptTable &opts);

public:
  //== Utils ==//
  static std::unique_ptr<llvm::raw_fd_ostream>
  GetFileOutputStream(llvm::StringRef outputFilename, Context &ctx);
};

} // namespace stone

#endif
