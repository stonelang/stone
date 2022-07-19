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

class FrontendStats final : public Stats {
  Frontend &frontend;

public:
  FrontendStats(Frontend &frontend)
      : Stats("Frontend statistics:"), frontend(frontend) {}
  void Print(ColorfulStream &stream) override;
};

struct ModuleBuffers {

  std::unique_ptr<llvm::MemoryBuffer> moduleBuffer;
  std::unique_ptr<llvm::MemoryBuffer> moduleDocBuffer;
  std::unique_ptr<llvm::MemoryBuffer> moduleSourceInfoBuffer;

  // Constructor
  ModuleBuffers(
      std::unique_ptr<llvm::MemoryBuffer> moduleBuffer,
      std::unique_ptr<llvm::MemoryBuffer> moduleDocBuffer = nullptr,
      std::unique_ptr<llvm::MemoryBuffer> moduleSourceInfoBuffer = nullptr)
      : moduleBuffer(std::move(moduleBuffer)),
        moduleDocBuffer(std::move(moduleDocBuffer)),
        moduleSourceInfoBuffer(std::move(moduleSourceInfoBuffer)) {}
};

class Frontend final : public Session {

  FrontendListener *listener = nullptr;
  llvm::StringRef programName;
  llvm::StringRef programPath;

  std::unique_ptr<FrontendOptions> frontendOpts;

  std::unique_ptr<FrontendStats> stats;

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
  llvm::SetVector<unsigned> primarySourceIDs;

  /// Allocator FrontendUnit
  mutable llvm::BumpPtrAllocator bumpAlloc;

  llvm::MemoryBuffer *codeCompletionBuffer = nullptr;
  /// Code completion offset in bytes from the beginning of the main
  /// source file.  Valid only if \c isCodeCompletion() == true.
  unsigned codeCompletionOffset = ~0U;

public:
  Frontend(llvm::StringRef programName, llvm::StringRef programPath,
           FrontendListener *listener = nullptr);
  ~Frontend();

public:
  // llvm::ArrayRef<FrontendUnit *> BuildSources(const file::Files &inputs);
  // FrontendUnit *BuildSource(const file::File &input);

  Error CreateSourceBuffers();

  // TODO: You may not need this anymore
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

  void Finish() override;

  // TODO: update FrontendOptions
  void ComputeModuleOutputMode() { assert(false && "Not implemented"); }

public:
  FrontendOptions &GetFrontendOptions() { return *frontendOpts.get(); }
  const FrontendOptions &GetFrontendOptions() const {
    return *frontendOpts.get();
  }

  CodeGenOptions &GetCodeGenOptions() { return codeGenOpts; }
  const CodeGenOptions &GetCodeGenOptions() const { return codeGenOpts; }

  TargetOptions &GetTargetOptions() { return targetOpts; }
  const TargetOptions &GetTargetOptions() const { return targetOpts; }

  TypeCheckerOptions &GetTypeCheckerOptions() { return typeCheckerOpts; }
  const TypeCheckerOptions &GetTypeCheckerOptions() const {
    return typeCheckerOpts;
  }

  SearchPathOptions &GetSearchPathOptions() { return searchPathOpts; }
  const SearchPathOptions &GetSearchPathOptions() const {
    return searchPathOpts;
  }

  TypeCheckMode GetTypeCheckMode() {
    return (primarySourceIDs.empty() ? TypeCheckMode::WholeModule
                                     : TypeCheckMode::EachFile);
    // TODO: Set in ParseArgs return GetTypeCheckerOptions().typeCheckMode;
  }

  FrontendListener *GetListener() { return listener; }
  void SetListener(FrontendListener *l) { listener = l; }

  Optional<ModuleBuffers>
  GetInputBuffersIfPresent(const FrontendInputFile &input);
  Optional<unsigned> GetRecordedBufferID(const FrontendInputFile &input,
                                         const bool shouldRecover,
                                         bool &failed);

  bool HasError() { return GetContext().GetDiagUnit().HasError(); }

  bool JustFrontend() {
    if (GetFrontendOptions().GetMode().JustParse() ||
        GetFrontendOptions().GetMode().JustTypeCheck() ||
        GetFrontendOptions().GetMode().IsEmitIR()) {
      return true;
    }
    return false;
  }

  std::vector<unsigned> &GetSourceBufferIDs() { return sourceBufferIDs; }
};

} // namespace stone

#endif
