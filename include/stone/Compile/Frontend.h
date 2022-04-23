#ifndef STONE_COMPILE_LANG_H
#define STONE_COMPILE_LANG_H

#include "stone/Compile/FrontendBase.h"
#include "stone/Compile/FrontendUnit.h"
#include "stone/Compile/ModuleSystem.h"
#include "stone/Compile/PackageSystem.h"
#include "stone/Gen/CodeGenContext.h"
#include "stone/Sem/TypeCheckerListener.h"
#include "stone/Sem/TypeCheckerOptions.h"
#include "stone/Session/Mode.h"
#include "stone/Syntax/Module.h"
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

// TODO: SmallString<128> workingDirectory;
// llvm::sys::fs::current_path(workingDirectory);

// class FrontendBase : public Session {};

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
  /// Print the lanuage help
  void PrintHelp();

  /// Print the language version
  void PrintVersion();

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
