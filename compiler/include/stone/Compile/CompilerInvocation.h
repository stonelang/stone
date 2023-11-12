#ifndef STONE_COMPILE_COMPILERINVOCATION_H
#define STONE_COMPILE_COMPILERINVOCATION_H

#include "stone/Basic/CodeGenOptions.h"
#include "stone/Basic/FileSystemOptions.h"
#include "stone/Basic/Mem.h"
#include "stone/Basic/ModuleOptions.h"
#include "stone/Basic/SrcLoc.h"
#include "stone/Compile/CompilerOptions.h"
#include "stone/Compile/ModuleSystem.h"
#include "stone/Gen/CodeGenContext.h"
#include "stone/Option/Action.h"
#include "stone/Public.h"
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

using ConfigurationFileBuffers =
    llvm::SmallVector<std::unique_ptr<llvm::MemoryBuffer>, 4>;

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

using MemoryBuffers =
    llvm::SmallVectorImpl<std::unique_ptr<llvm::MemoryBuffer>>;

class CompilerInvocation {

  CompilerOptions compilerOpts;

  /// Options for generating code
  CodeGenOptions codeGenOpts;

  /// The options for searching libs
  SearchPathOptions searchPathOpts;

  /// The options for type-checking
  TypeCheckerOptions typeCheckerOpts;

  LangTargetOptions targetOpts;

  ModuleOptions moduleOpts;

  SyntaxOptions syntaxOpts;

  /// Contains buffer IDs for input source code files.
  std::vector<unsigned> sourceBufferIDs;

  // The primary Sources
  llvm::SetVector<unsigned> primarySourceIDs;

  llvm::MemoryBuffer *codeCompletionBuffer = nullptr;
  /// Code completion offset in bytes from the beginning of the main
  /// source file.  Valid only if \c isCodeCompletion() == true.
  unsigned codeCompletionOffset = ~0U;

  mutable llvm::BumpPtrAllocator bumpAlloc;

  LangContext langContext;

  std::unique_ptr<ClangContext> clangContext;

public:
  CompilerInvocation();
  ~CompilerInvocation();

public:
  Status ParseOptions(llvm::ArrayRef<const char *> args);

public:
  // llvm::ArrayRef<CompilerUnit *> BuildSources(const file::Files &inputs);
  // CompilerUnit *BuildSource(const file::File &input);

  Status CreateSourceBuffers();

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

  // std::unique_ptr<OutputFile> ComputeOutputFile(CompilerUnit &source);

  void Finish();

  // TODO: update CompilerOptions
  void ComputeModuleOutputMode() { assert(false && "Not implemented"); }
  Status SetupClang(llvm::ArrayRef<const char *> args, const char *arg0);

public:
  void SetTargetTriple(llvm::StringRef triple);
  void SetTargetTriple(const llvm::Triple &Triple);

  LangContext &GetLangContext() { return langContext; }

  ClangContext &GetClangContext() { return *clangContext; }

  CompilerOptions &GetCompilerOptions() { return compilerOpts; }
  const CompilerOptions &GetCompilerOptions() const { return compilerOpts; }

  CodeGenOptions &GetCodeGenOptions() { return codeGenOpts; }
  const CodeGenOptions &GetCodeGenOptions() const { return codeGenOpts; }

  LangTargetOptions &GetTargetOptions() { return targetOpts; }
  const LangTargetOptions &GetTargetOptions() const { return targetOpts; }

  SyntaxOptions &GetSyntaxOptions() { return syntaxOpts; }
  const SyntaxOptions &GetSyntaxOptions() const { return syntaxOpts; }

  TypeCheckerOptions &GetTypeCheckerOptions() { return typeCheckerOpts; }
  const TypeCheckerOptions &GetTypeCheckerOptions() const {
    return typeCheckerOpts;
  }

  LangOptions &GetLangOptions() { return GetLangContext().GetLangOptions(); }
  const LangOptions &GetLangOptions() const {
    return GetLangContext().GetLangOptions();
  }

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

  DiagnosticEngine &GetDiags() { GetLangContext().GetDiags(); }

  Optional<ModuleBuffers>
  GetInputBuffersIfPresent(const CompilerInputFile &input);
  Optional<unsigned> GetRecordedBufferID(const CompilerInputFile &input,
                                         const bool shouldRecover,
                                         bool &failed);

  llvm::BumpPtrAllocator &GetBumpAllocator() { return bumpAlloc; }

  bool HasError() { return GetLangContext().GetDiags().HasError(); }

  std::vector<unsigned> &GetSourceBufferIDs() { return sourceBufferIDs; }
  // std::vector<unsigned> &GetPrimarySourceIDs() { return primarySourceIDs; }

  CompilerAction &GetAction() { return GetCompilerOptions().GetAction(); }

private:
  Status ParseCompilerAction(llvm::opt::InputArgList &args);
  Status ParseCompilerOptions(llvm::opt::InputArgList &args,
                              MemoryBuffers *buffers);
  Status ParseLangOptions(llvm::opt::InputArgList &args);
  Status ParseTypeCheckerOptions(llvm::opt::InputArgList &args);
  Status ParseSearchPathOptions(llvm::opt::InputArgList &args);
  Status ParseCodeGenOptions(llvm::opt::InputArgList &args);
  Status ParseTargetOptions(llvm::opt::InputArgList &args);
  llvm::StringRef ParseWorkDirectory(const llvm::opt::InputArgList &args);

public:
  void PrintHelp();
};

} // namespace stone

#endif
