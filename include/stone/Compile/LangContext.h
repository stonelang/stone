#ifndef STONE_LANG_LANGCONTEXT_H
#define STONE_LANG_LANGCONTEXT_H

#include "stone/Compile/LangOptions.h"
#include "stone/Compile/SourceUnit.h"
#include "stone/Compile/TypeCheckerOptions.h"
#include "stone/Core/Context.h"
#include "stone/Core/FileSystemOptions.h"
#include "stone/Core/Mem.h"
#include "stone/Core/SrcLoc.h"
#include "stone/Gen/CodeGenOptions.h"
#include "stone/Option/OptUtil.h"
#include "stone/Syntax/SearchPathOptions.h"

#include "llvm/Option/ArgList.h"

namespace stone {

class LangContext final {

  friend class Lang;

  LangOptions langOpts;

  // TODO: May move to Options 
  opts::OptUtil optUtil;

  /// The main executable path of the running program
  std::string mainExecutablePath;

  /// The system context
  Context ctx;

  /// The translated arguments.
  std::unique_ptr<llvm::opt::DerivedArgList> dal;

  // Let use use a map for the time being.
  std::map<unsigned, SourceUnit *> sources;

  /// Allocator SourceUnit
  mutable llvm::BumpPtrAllocator bumpAlloc;

public:
  LangContext();
  ~LangContext();

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

public:
  Context &GetContext() { return ctx; }

  opts::OptUtil &GetOptUtil() { return optUtil; }

  Mode &GetMode() { return optUtil.GetMode(); }
  const Mode &GetMode() const { return optUtil.GetMode(); }

  file::Files &GetInputFiles() { return optUtil.inputFiles; }

  SourceUnit *GetSourceUnit(const unsigned srcID) { return sources[srcID]; }

  std::unique_ptr<OutputFile> ComputeOutputFile(const unsigned srcID);

public:
  bool ParseArgs(llvm::ArrayRef<const char *> args);
  unsigned CreateSourceBuffer(const file::File &input);

  void BuildSources(file::Files &files);

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

private:
  SourceUnit *BuildSourceUnit(const file::File &input);
};

} // namespace stone

inline void *operator new(size_t bytes, const stone::LangContext &lc,
                          size_t alignment) {
  return lc.Allocate(bytes, alignment);
}

/// Placement delete companion to the new above.
///
/// This operator is just a companion to the new above. There is no way of
/// invoking it directly; see the new operator for more details. This operator
/// is called implicitly by the compiler if a placement new expression using
/// the CompilationInvocation throws in the object constructor.
inline void operator delete(void *Ptr, const stone::LangContext &lc, size_t) {
  lc.Deallocate(Ptr);
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
inline void *operator new[](size_t bytes, const stone::LangContext &lc,
                            size_t alignment) {
  return lc.Allocate(bytes, alignment);
}
/// Placement delete[] companion to the new[] above.
///
/// This operator is just a companion to the new[] above. There is no way of
/// invoking it directly; see the new[] operator for more details. This operator
/// is called implicitly by the compiler if a placement new[] expression using
/// the CompilationInvocation throws in the object constructor.
inline void operator delete[](void *Ptr, const stone::LangContext &lc,
                              size_t alignment) {
  lc.Deallocate(Ptr);
}

#endif