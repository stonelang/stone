#ifndef STONE_COMPILE_COMPILER_H
#define STONE_COMPILE_COMPILER_H

#include "stone/Compile/CompilableFile.h"
#include "stone/Compile/CompilerAlloc.h"
#include "stone/Compile/CompilerOptions.h"

#include "stone/Session/Session.h"
#include "stone/Syntax/Module.h"
#include "stone/Syntax/SearchPathOptions.h"
#include "stone/Syntax/Syntax.h"
#include "stone/Syntax/TreeContext.h"

#include "llvm/ADT/DenseMap.h"
#include "llvm/Support/BuryPointer.h"

#include <cassert>
#include <list>
#include <memory>
#include <string>
#include <utility>

namespace llvm {
class raw_fd_ostream;
class Timer;
class TimerGroup;
} // namespace llvm

using namespace stone::syn;

namespace stone {
class Compiler;
class CompilerListener;

class CompilerStats final : public Stats {
  Compiler &compiler;

public:
  CompilerStats(Compiler &compiler, Basic &basic)
      : Stats("compiler statistics:", basic), compiler(compiler) {}
  void Print() override;
};

class Compiler final : public Session {
  SrcMgr sm;
  CompilerListener *cl = nullptr;
  mutable syn::Module *mainModule = nullptr;

  std::unique_ptr<TreeContext> tc;

  friend CompilerStats;
  std::unique_ptr<CompilerStats> stats;
  std::unique_ptr<Syntax> syntax;

  /// If the output doesn't support seeking (terminal, pipe). we switch
  /// the stream to a buffer_ostream. These are the buffer and the original
  /// stream.
  std::unique_ptr<llvm::raw_fd_ostream> nonSeekStream;

  /// Force an output buffer.
  std::unique_ptr<llvm::raw_pwrite_stream> outputStream;

private:
  static int Run(Compiler &compiler);
  /*
          /// Identifies the set of input buffers in the SrcMgr that are
    /// considered main source files.
    llvm::SetVector<unsigned> primaryBufferIDs;

          /// Return whether there is an entry in PrimaryInputs for buffer \p
    BufID. bool IsPrimaryInput(SrcID fileID) const { return
    primaryBufferIDs.count(fileID) != 0;
    }

    /// Record in PrimaryBufferIDs the fact that \p BufID is a primary.
    /// If \p BufID is already in the set, do nothing.
    void RecordPrimaryInputBuffer(SrcID fileID);
  */
public:
  CompilerOptions compilerOpts;

public:
  Compiler(const Compiler &) = delete;
  void operator=(const Compiler &) = delete;
  explicit Compiler(CompilerListener *cl = nullptr);

public:
  void Init() override;
  /// Parse the given list of strings into an InputArgList.
  bool Build(llvm::ArrayRef<const char *> args) override;

  int Run() override;
  /// Parse the given list of strings into an InputArgList.
  void PrintLifecycle() override;
  void PrintHelp(bool showHidden) override;

  SearchPathOptions &GetSearchPathOptions() { return compilerOpts.spOpts; }
  const SearchPathOptions &GetSearchPathOptions() const {
    return compilerOpts.spOpts;
  }

  CompilerOptions &GetCompilerOptions() { return compilerOpts; }
  const CompilerOptions &GetCompilerOptions() const { return compilerOpts; }

  SrcMgr &GetSrcMgr() { return sm; }

  llvm::vfs::FileSystem &GetVFS() const;

  TreeContext &GetTreeContext() { return *tc.get(); }

  Syntax &GetSyntax() { return *syntax.get(); }

  /// Retrieve the main module containing the files being compiled.
  syn::Module *GetMainModule() const;
  /// Replace the current main module with a new one. This is used for top-level
  /// cached code completion.
  void SetMainModule(syn::Module *mainModule);

  void SetInputType(file::Type ty) { compilerOpts.inputType = ty; }
  file::Type GetInputKind() const { return compilerOpts.inputType; }

  // TODO:
  void SetModuleName(llvm::StringRef name) { compilerOpts.moduleName = name; }
  const llvm::StringRef GetModuleName() const {
    return compilerOpts.moduleName;
  }

  CompilerStats &GetStats() { return *stats.get(); }

  void SetCompilerListener(CompilerListener *listener) { cl = listener; }
  CompilerListener *GetCompilerListener() { return cl; }

  std::unique_ptr<raw_pwrite_stream>
  CreateOutputFile(llvm::StringRef outFile, bool isBinary,
                   bool removeFileOnSignal, StringRef inFile,
                   llvm::StringRef extension, bool useTemporary,
                   bool createMissingDirectories);

  // TODO: Move to File::CreateOutputFile()
  std::unique_ptr<llvm::raw_pwrite_stream>
  CreateOutputFile(llvm::StringRef outFile, std::error_code &error,
                   bool isBinary, bool removeFileOnSignal,
                   llvm::StringRef inFile, StringRef extension,
                   bool useTemporary, bool CreateMissingDirectories,
                   std::string *resultPathName, std::string *tempPathName);

protected:
  void ComputeMode(const llvm::opt::DerivedArgList &args) override;
  ModeType GetDefaultModeType() override;
  void BuildOptions() override;

  llvm::StringRef GetName() override { return "Stone compiler "; }
  llvm::StringRef GetDescription() override {
    return "Stone compiler front-end ";
  }

  /// TranslateInputArgs - Create a new derived argument list from the input
  /// arguments, after applying the standard argument translations.
  // llvm::opt::DerivedArgList *
  // TranslateInputArgs(const llvm::opt::InputArgList &args) override const;

public:
  void *Allocate(size_t size, unsigned align) const {
    return bumpAlloc.Allocate(size, align);
  }

  // TODO: You may need to do this.
  // void Reset() { bumpAlloc.Reset()); }

  template <typename T> T *Allocate(size_t num = 1) const {
    return static_cast<T *>(Allocate(num * sizeof(T), alignof(T)));
  }
  void Deallocate(void *ptr) const {}

public:
  template <typename UnitTy, typename AllocatorTy>
  static void *Allocate(AllocatorTy &alloc, size_t baseSize) {
    static_assert(alignof(UnitTy) >= sizeof(void *),
                  "A pointer must fit in the alignment of the InputFile!");

    return (void *)alloc.Allocate(baseSize, alignof(UnitTy));
  }
};
} // namespace stone

inline void *operator new(size_t bytes, const stone::Compiler &compiler,
                          size_t alignment) {
  return compiler.Allocate(bytes, alignment);
}

/// Placement delete companion to the new above.
///
/// This operator is just a companion to the new above. There is no way of
/// invoking it directly; see the new operator for more details. This operator
/// is called implicitly by the compiler if a placement new expression using
/// the CompilationInvocation throws in the object constructor.
inline void operator delete(void *Ptr, const stone::Compiler &compiler,
                            size_t) {
  compiler.Deallocate(Ptr);
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
inline void *operator new[](size_t bytes, const stone::Compiler &compiler,
                            size_t alignment) {
  return compiler.Allocate(bytes, alignment);
}

/// Placement delete[] companion to the new[] above.
///
/// This operator is just a companion to the new[] above. There is no way of
/// invoking it directly; see the new[] operator for more details. This operator
/// is called implicitly by the compiler if a placement new[] expression using
/// the CompilationInvocation throws in the object constructor.
inline void operator delete[](void *Ptr, const stone::Compiler &compiler,
                              size_t alignment) {
  compiler.Deallocate(Ptr);
}
#endif
