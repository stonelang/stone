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

  bool HasError() { return GetContext().GetDiagUnit().HasError(); }

  bool JustFrontend() {
    if (GetMode().JustParse() || GetMode().JustTypeCheck() ||
        GetMode().IsEmitIR()) {
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

  Frontend(FrontendListener *listener = nullptr);
  ~Frontend();

public:
  void Initialize();
  void Finish();

  llvm::opt::InputArgList &
  ParseArgs(llvm::ArrayRef<const char *> args) override;

  stone::Error ParseArguments(llvm::ArrayRef<const char *> args);

public:
  syn::Syntax &GetSyntax() { return *syntax.get(); }
  ModuleSystem &GetModuleSystem() { return *moduleSystem.get(); }

  PackageSystem &GetPackageSystem() { return *pkgSystem.get(); }

  // llvm::StringRef CreateOutputFile(unsigned srcID);
  llvm::StringRef ComputeSourceOutputFile(unsigned srcID);

  FrontendListener *GetListener() { return listener; }
  void SetListener(FrontendListener *l) { listener = l; }


public:
  /// Perform code analysis and code generation
  void Compile(llvm::ArrayRef<FrontendUnit *> &sources);

private:
  void CompileWithSyntaxAnalysis(llvm::ArrayRef<FrontendUnit *> &sources);
  void CompileWithSyntaxAnalysis(llvm::ArrayRef<FrontendUnit *> &sources,
                                 SyntaxAnalysisCallback client);

  void CompileWithSemanticAnalysis(llvm::ArrayRef<FrontendUnit *> &sources);
  void CompileWithSemanticAnalysis(llvm::ArrayRef<FrontendUnit *> &sources,
                                   SemanticAnalysisCallback client);

  void ForEachSyntaxFile(EachSyntaxFileCallback client);

  void ResolveUsings();

public:
  ModuleOutputMode GetModuleOutputMode() {
    // TODO: This must be computed in the future.
    return GetModuleSystem().GetModuleOptions().moduleOutputMode;
  }

void PrintHelp(const llvm::opt::OptTable& opts);

public:
  //== Utils ==//
  static std::unique_ptr<llvm::raw_fd_ostream>
  GetFileOutputStream(llvm::StringRef outputFilename, Context &ctx);

public:
  void *Allocate(size_t Size, unsigned Align) const {
    return bumpAlloc.Allocate(Size, Align);
  }
  template <typename T> T *Allocate(size_t Num = 1) const {
    return static_cast<T *>(Allocate(Num * sizeof(T), alignof(T)));
  }
  void Deallocate(void *ptr) const {}

public:
  template <typename ProfileTy, typename AllocatorTy>
  static void *Allocate(AllocatorTy &A, size_t BaseSize) {
    static_assert(alignof(ProfileTy) >= sizeof(void *),
                  "A pointer must fit in the alignment of the ModeUnit!");

    return (void *)A.Allocate(BaseSize, alignof(ProfileTy));
  }
};



} // namespace stone

inline void *operator new(size_t bytes, const stone::Frontend &frontend,
                          size_t alignment) {
  return frontend.Allocate(bytes, alignment);
}

/// Placement delete companion to the new above.
///
/// This operator is just a companion to the new above. There is no way of
/// invoking it directly; see the new operator for more details. This operator
/// is called implicitly by the compiler if a placement new expression using
/// the CompilationInvocation throws in the object constructor.
inline void operator delete(void *Ptr, const stone::Frontend &frontend,
                            size_t) {
  frontend.Deallocate(Ptr);
}
/// This placement form of operator new[] uses the CompilerInstance's
/// allocator for obtaining memory.
///
/// We intentionally avoid using a nothrow specification here so that the calls
/// to this operator will not perform a null check on the result -- the
/// underlying allocator never returns null pointers.
///
/// Usage looks like this (assuming there's an CompilationInvocation
/// 'Invocation' in scope):
/// @code
/// // Default alignment (8)
/// char *data = new (Invocation) char[10];
/// // Specific alignment
/// char *data = new (Invocation, 4) char[10];
/// @endcode
/// Memory allocated through this placement new[] operator does not need to be
/// explicitly freed, as CompilationInvocation will free all of this memory when
/// it gets destroyed. Please note that you cannot use delete on the pointer.
///
/// @param Bytes The number of bytes to allocate. Calculated by the compiler.
/// @param C The CompilationInvocation that provides the allocator.
/// @param Alignment The alignment of the allocated memory (if the underlying
///                  allocator supports it).
/// @return The allocated memory. Could be nullptr.
inline void *operator new[](size_t bytes, const stone::Frontend &frontend,
                            size_t alignment) {
  return frontend.Allocate(bytes, alignment);
}
/// Placement delete[] companion to the new[] above.
///
/// This operator is just a companion to the new[] above. There is no way of
/// invoking it directly; see the new[] operator for more details. This operator
/// is called implicitly by the compiler if a placement new[] expression using
/// the CompilationInvocation throws in the object constructor.
inline void operator delete[](void *Ptr, const stone::Frontend &frontend,
                              size_t alignment) {
  frontend.Deallocate(Ptr);
}

#endif
