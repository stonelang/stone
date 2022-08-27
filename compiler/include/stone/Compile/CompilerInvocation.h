#ifndef STONE_COMPILE_FRONTEND_H
#define STONE_COMPILE_FRONTEND_H

#include "stone/Basic/CodeGenOptions.h"
#include "stone/Basic/FileSystemOptions.h"
#include "stone/Basic/Mem.h"
#include "stone/Basic/ModuleOptions.h"
#include "stone/Basic/SrcLoc.h"
#include "stone/Compile/CompilerOptions.h"
#include "stone/Compile/CompilerUnit.h"
#include "stone/Compile/ModuleSystem.h"
#include "stone/Compile/PackageSystem.h"
#include "stone/Context.h"
#include "stone/Gen/CodeGenContext.h"
#include "stone/Sem/TypeCheckerListener.h"
#include "stone/Sem/TypeCheckerOptions.h"
#include "stone/Session/Mode.h"
#include "stone/Session/Session.h"
#include "stone/Syntax/Module.h"
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
class CompilerListener;
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

class CompilerInvocation final : public Session {

  CompilerListener *listener = nullptr;
  llvm::StringRef programName;
  llvm::StringRef programPath;

  std::unique_ptr<CompilerOptions> invocationOpts;

  /// Options for generating code
  CodeGenOptions codeGenOpts;

  /// The options for searching libs
  SearchPathOptions searchPathOpts;

  /// The options for type-checking
  sem::TypeCheckerOptions typeCheckerOpts;

  TargetOptions targetOpts;

  /// The main executable path of the running program
  std::string mainExecutablePath;

  /// Contains buffer IDs for input source code files.
  std::vector<unsigned> sourceBufferIDs;

  // The primary Sources
  llvm::SetVector<unsigned> primarySourceIDs;

  llvm::MemoryBuffer *codeCompletionBuffer = nullptr;
  /// Code completion offset in bytes from the beginning of the main
  /// source file.  Valid only if \c isCodeCompletion() == true.
  unsigned codeCompletionOffset = ~0U;

  mutable llvm::BumpPtrAllocator bumpAlloc;

public:
  CompilerInvocation(llvm::StringRef programName, llvm::StringRef programPath,
                     CompilerListener *listener = nullptr);
  ~CompilerInvocation();

public:
  // llvm::ArrayRef<CompilerUnit *> BuildSources(const file::Files &inputs);
  // CompilerUnit *BuildSource(const file::File &input);

  Error CreateSourceBuffers();

  // TODO: You may not need this anymore
  unsigned CreateSourceBuffer(const CompilerInputFile &input);

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

  // std::unique_ptr<OutputFile> ComputeOutputFile(CompilerUnit &source);

  void Finish() override;

  // TODO: update CompilerOptions
  void ComputeModuleOutputMode() { assert(false && "Not implemented"); }

public:
  CompilerOptions &GetCompilerOptions() { return *invocationOpts.get(); }
  const CompilerOptions &GetCompilerOptions() const {
    return *invocationOpts.get();
  }

  CodeGenOptions &GetCodeGenOptions() { return codeGenOpts; }
  const CodeGenOptions &GetCodeGenOptions() const { return codeGenOpts; }

  TargetOptions &GetTargetOptions() { return targetOpts; }
  const TargetOptions &GetTargetOptions() const { return targetOpts; }

  sem::TypeCheckerOptions &GetTypeCheckerOptions() { return typeCheckerOpts; }
  const sem::TypeCheckerOptions &GetTypeCheckerOptions() const {
    return typeCheckerOpts;
  }

  SearchPathOptions &GetSearchPathOptions() { return searchPathOpts; }
  const SearchPathOptions &GetSearchPathOptions() const {
    return searchPathOpts;
  }

  sem::TypeCheckMode GetTypeCheckMode() {
    return (primarySourceIDs.empty() ? sem::TypeCheckMode::WholeModule
                                     : sem::TypeCheckMode::EachFile);
    // TODO: Set in ParseArgs return GetTypeCheckerOptions().typeCheckMode;
  }

  CompilerListener *GetListener() { return listener; }
  void SetListener(CompilerListener *l) { listener = l; }

  Optional<ModuleBuffers>
  GetInputBuffersIfPresent(const CompilerInputFile &input);
  Optional<unsigned> GetRecordedBufferID(const CompilerInputFile &input,
                                         const bool shouldRecover,
                                         bool &failed);

  llvm::BumpPtrAllocator &GetMemAllocator() { return bumpAlloc; }

  bool HasError() { return GetContext().GetDiagUnit().HasError(); }

  bool JustCompiler() {
    if (GetCompilerOptions().GetMode().JustParse() ||
        GetCompilerOptions().GetMode().JustTypeCheck() ||
        GetCompilerOptions().GetMode().IsEmitIR()) {
      return true;
    }
    return false;
  }

  std::vector<unsigned> &GetSourceBufferIDs() { return sourceBufferIDs; }
};

} // namespace stone

#endif
