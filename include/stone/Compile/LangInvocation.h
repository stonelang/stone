#ifndef STONE_COMPILE_FRONTEND_H
#define STONE_COMPILE_FRONTEND_H

#include "stone/Compile/LangOptions.h"
#include "stone/Compile/SourceUnit.h"
#include "stone/Compile/TypeCheckerOptions.h"
#include "stone/Core/Context.h"
#include "stone/Core/FileSystemOptions.h"
#include "stone/Core/Mem.h"
#include "stone/Core/SrcLoc.h"
#include "stone/Gen/CodeGenOptions.h"
#include "stone/Session/Session.h"
#include "stone/Syntax/SearchPathOptions.h"

#include "llvm/Option/ArgList.h"

namespace stone {

class LangInvocation final : public Session {
  friend class LangInstance;
  LangOptions langOpts;

  /// The main executable path of the running program
  std::string mainExecutablePath;
  llvm::SmallVector<SourceUnit *> sources;

  /// Allocator SourceUnit
  mutable llvm::BumpPtrAllocator bumpAlloc;

public:
  LangInvocation();
  ~LangInvocation();

public:
  llvm::opt::InputArgList &
  ParseArgs(llvm::ArrayRef<const char *> args) override;
  llvm::ArrayRef<SourceUnit *> BuildSources(const file::Files &inputs);
  SourceUnit *BuildSource(const file::File &input);
  unsigned CreateSourceID(const file::File &input);

  BaseOptions &GetBaseOptions() override { return langOpts; }
  file::Files &GetInputFiles() { return langOpts.inputFiles; }
  std::unique_ptr<OutputFile> ComputeOutputFile(SourceUnit &source);

  // TODO: Move to the module system
  void SetModuleName(llvm::StringRef name) {
    GetLangOptions().systemOpts.moduleName = name.data();
  }
  const llvm::StringRef GetModuleName() const {
    return GetLangOptions().systemOpts.moduleName;
  }

public:
  void SetMainExecutablePath(std::string path) { mainExecutablePath = path; }
  std::string GetMainExecutablePath() const { return mainExecutablePath; }

  LangOptions &GetLangOptions() { return langOpts; }
  const LangOptions &GetLangOptions() const { return langOpts; }

  CodeGenOptions &GetCodeGenOptions() { return langOpts.codeGenOpts; }
  const CodeGenOptions &GetCodeGenOptions() const {
    return langOpts.codeGenOpts;
  }

  TypeCheckerOptions &GetTypeCheckerOptions() {
    return langOpts.typeCheckerOpts;
  }
  const TypeCheckerOptions &GetTypeCheckerOptions() const {
    return langOpts.typeCheckerOpts;
  }

  TypeCheckMode GetTypeCheckMode() {
    return GetTypeCheckerOptions().typeCheckMode;
  }

  bool HasError() { return GetContext().GetDiagEngine().HasError(); }

  bool JustAnalysis() {
    if (GetMode().JustParse() || GetMode().JustTypeCheck()) {
      return true;
    }
    return false;
  }
  bool CanCodeGen() {
    switch (GetMode().GetKind()) {
    case ModeKind::None:
    case ModeKind::EmitIR:
    case ModeKind::EmitBC:
    case ModeKind::EmitObject:
    case ModeKind::EmitAssembly:
    case ModeKind::EmitModule:
    case ModeKind::EmitLibrary:
      return true;
    default:
      return false;
    }
  }

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

inline void *operator new(size_t bytes,
                          const stone::LangInvocation &langInvocation,
                          size_t alignment) {
  return langInvocation.Allocate(bytes, alignment);
}

/// Placement delete companion to the new above.
///
/// This operator is just a companion to the new above. There is no way of
/// invoking it directly; see the new operator for more details. This operator
/// is called implicitly by the compiler if a placement new expression using
/// the CompilationInvocation throws in the object constructor.
inline void operator delete(void *Ptr,
                            const stone::LangInvocation &langInvocation,
                            size_t) {
  langInvocation.Deallocate(Ptr);
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
inline void *operator new[](size_t bytes,
                            const stone::LangInvocation &langInvocation,
                            size_t alignment) {
  return langInvocation.Allocate(bytes, alignment);
}
/// Placement delete[] companion to the new[] above.
///
/// This operator is just a companion to the new[] above. There is no way of
/// invoking it directly; see the new[] operator for more details. This operator
/// is called implicitly by the compiler if a placement new[] expression using
/// the CompilationInvocation throws in the object constructor.
inline void operator delete[](void *Ptr,
                              const stone::LangInvocation &langInvocation,
                              size_t alignment) {
  langInvocation.Deallocate(Ptr);
}

#endif