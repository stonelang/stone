#ifndef STONE_COMPILE_COMPILERINVOCATION_H
#define STONE_COMPILE_COMPILERINVOCATION_H

#include "stone/Basic/CodeGenOptions.h"
#include "stone/Basic/FileSystemOptions.h"
#include "stone/Basic/Mem.h"
#include "stone/Basic/ModuleOptions.h"
#include "stone/Basic/SrcLoc.h"
#include "stone/CodeCompletionListener.h"
#include "stone/Compile/CompilerOptions.h"
#include "stone/Compile/ModuleSystem.h"
#include "stone/Gen/CodeGenContext.h"
#include "stone/Public.h"
#include "stone/Session/Mode.h"
#include "stone/Session/Session.h"
#include "stone/Syntax/Module.h"
#include "stone/Syntax/SyntaxContext.h"
#include "stone/Syntax/SyntaxOptions.h"
#include "stone/Syntax/TypeCheckerOptions.h"

#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/CompilerInvocation.h"

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

  std::unique_ptr<CompilerOptions> compilerOpts;

  /// Options for generating code
  CodeGenOptions codeGenOpts;

  /// The options for searching libs
  SearchPathOptions searchPathOpts;

  /// The options for type-checking
  TypeCheckerOptions typeCheckerOpts;

  stone::TargetOptions targetOpts;

  LangOptions langOpts;

  ModuleOptions moduleOpts;

  SyntaxOptions syntaxOpts;

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

  std::unique_ptr<ClangContext> clangContext;

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
  stone::Error SetupClang(llvm::ArrayRef<const char *> args, const char *arg0);

public:
  void SetTargetTriple(llvm::StringRef triple);
  void SetTargetTriple(const llvm::Triple &Triple);

  ClangContext &GetClangContext() { return *clangContext; }

  CompilerOptions &GetCompilerOptions() { return *compilerOpts.get(); }
  const CompilerOptions &GetCompilerOptions() const {
    return *compilerOpts.get();
  }

  CodeGenOptions &GetCodeGenOptions() { return codeGenOpts; }
  const CodeGenOptions &GetCodeGenOptions() const { return codeGenOpts; }

  stone::TargetOptions &GetTargetOptions() { return targetOpts; }
  const stone::TargetOptions &GetTargetOptions() const { return targetOpts; }

  SyntaxOptions &GetSyntaxOptions() { return syntaxOpts; }
  const SyntaxOptions &GetSyntaxOptions() const { return syntaxOpts; }

  TypeCheckerOptions &GetTypeCheckerOptions() { return typeCheckerOpts; }
  const TypeCheckerOptions &GetTypeCheckerOptions() const {
    return typeCheckerOpts;
  }

  LangOptions &GetLangOptions() { return langOpts; }
  const LangOptions &GetLangOptions() const { return langOpts; }
  SearchPathOptions &GetSearchPathOptions() { return searchPathOpts; }
  const SearchPathOptions &GetSearchPathOptions() const {
    return searchPathOpts;
  }
  ModuleOptions &GetModuleOptions() { return moduleOpts; }
  const ModuleOptions &GetModuleOptions() const { return moduleOpts; }

  TypeCheckMode GetTypeCheckMode() {
    return (primarySourceIDs.empty() ? TypeCheckMode::WholeModule
                                     : TypeCheckMode::EachFile);
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

  bool HasError() { return GetLangContext().GetDiagUnit().HasError(); }

  bool JustCompile() {
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
