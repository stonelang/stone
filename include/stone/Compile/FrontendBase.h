#ifndef STONE_COMPILE_FRONTEND_H
#define STONE_COMPILE_FRONTEND_H

#include "stone/Basic/CodeGenOptions.h"
#include "stone/Basic/Context.h"
#include "stone/Basic/FileSystemOptions.h"
#include "stone/Basic/Mem.h"
#include "stone/Basic/SrcLoc.h"
#include "stone/Compile/FrontendOptions.h"
#include "stone/Compile/FrontendUnit.h"
#include "stone/Sem/TypeCheckerOptions.h"
#include "stone/Session/Session.h"
#include "stone/Syntax/SearchPathOptions.h"

#include "llvm/ADT/SetVector.h"
#include "llvm/Option/ArgList.h"

namespace stone {

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

  // All sources
  llvm::SmallVector<FrontendUnit *, 32> sources;

  // The primary Sources
  llvm::SetVector<unsigned> primarySourceIDs;

  /// Allocator FrontendUnit
  mutable llvm::BumpPtrAllocator bumpAlloc;

  llvm::MemoryBuffer *codeCompletionBuffer = nullptr;
  /// Code completion offset in bytes from the beginning of the main
  /// source file.  Valid only if \c isCodeCompletion() == true.
  unsigned codeCompletionOffset = ~0U;

public:
  FrontendBase();
  ~FrontendBase();

public:
  llvm::ArrayRef<FrontendUnit *> BuildSources(const file::Files &inputs);
  FrontendUnit *BuildSource(const file::File &input);
  unsigned CreateSourceID(const file::File &input);

  /// Return whether there is an entry in PrimaryInputs for buffer \p BufID.
  bool IsPrimarySourceID(unsigned primarySourceID) const {
    return primarySourceIDs.count(primarySourceID) != 0;
  }
  void RecordPrimarySourceID(unsigned primarySourceID);

  /// Gets the set of SourceFiles which are the primary inputs for this
  /// CompilerInstance.
  // llvm::ArrayRef<syn::SyntaxFile *> GetPrimaryFiles() const {
  //   return GetModuleSystem().GetMainModule()->GetPrimaryFiles();
  // }

  BaseOptions &GetBaseOptions() override { return *frontendOpts; }

  file::Files &GetInputFiles() { return GetBaseOptions().inputFiles; }

  std::unique_ptr<OutputFile> ComputeOutputFile(FrontendUnit &source);

  // TODO: update FrontendOptions
  void ComputeModuleOutputMode() { assert(false && "Not implemented"); }

public:
  void SetMainExecutablePath(std::string path) { mainExecutablePath = path; }
  std::string GetMainExecutablePath() const { return mainExecutablePath; }

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
  ModuleOutputMode GetModuleOutputMode() {
    // TODO: This must be computed in the future.
    return GetFrontendOptions().moduleOutputMode;
  }

  bool HasError() { return GetContext().GetDiagEngine().HasError(); }

  bool JustFrontend() {
    if (GetMode().JustParse() || GetMode().JustTypeCheck() ||
        GetMode().IsEmitIR()) {
      return true;
    }
    return false;
  }
};

} // namespace stone
#endif