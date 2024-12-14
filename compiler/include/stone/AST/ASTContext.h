#ifndef STONE_AST_ASTCONTEXT_H
#define STONE_AST_ASTCONTEXT_H

#include "stone/AST/Builtin.h"
#include "stone/AST/DeclName.h"
#include "stone/AST/Identifier.h"
#include "stone/AST/Import.h"
#include "stone/AST/LangABI.h"
#include "stone/AST/SearchPath.h"
#include "stone/AST/Type.h"
#include "stone/Basic/LangOptions.h"
#include "stone/Basic/Memory.h"
#include "stone/Basic/SrcMgr.h"
#include "stone/Support/Statistics.h"

#include "stone/AST/ClangImporter.h"
#include "stone/AST/Diagnostics.h"
#include "stone/AST/Expr.h"
#include "stone/AST/Ownership.h"
#include "stone/AST/Type.h"
#include "stone/Basic/SrcLoc.h"

#include "llvm/ADT/APSInt.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/FoldingSet.h"
#include "llvm/ADT/IntrusiveRefCntPtr.h"
#include "llvm/ADT/MapVector.h"
#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/PointerUnion.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/TinyPtrVector.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/Support/AlignOf.h"
#include "llvm/Support/Allocator.h"
#include "llvm/Support/Casting.h"
#include "llvm/TargetParser/Triple.h"
// #include "llvm/Support/VirtualOutputBackend.h"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <memory>
#include <optional>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace stone {
class DiagnosticEngine;
class BlockExpr;
class LangABI;
class Decl;
class ConstructorDecl;
class MethodDecl;
class RecordDecl;
class Expr;
class MangleContext;
class Module;
class Stmt;
class Builtin;
class ASTContext;
class Decl;
class DeclContext;
class FunDecl;
class StructDecl;
class Stmt;
class IfStmt;
class SwitchStmt;
class Expr;
class StatsReporter;

/// Look up option used in \c GetRealModuleName when module aliasing is applied.
enum class ModuleAliasLookupOption {
  AlwaysRealName,
  RealNameFromAlias,
  AliasFromRealName
};

class ASTContext final {

  ClangImporter &clangImporter;

  /// The search path options
  const SearchPathOptions &searchPathOpts;

  DiagnosticEngine &de;

  LangOptions &langOpts;

  Builtin builtin;

  /// Table for all
  IdentifierTable identifiers;

  mutable llvm::BumpPtrAllocator allocator;

  mutable DeclNameTable declNames;

  /// All builtin types will be stored here.
  mutable llvm::SmallVector<Type *, 0> builtinTypes;

  /// The standard library module.
  mutable ModuleDecl *stdlibModule = nullptr;

  /// The name of the standard library module "libstone".
  // Identifier stdlibModuleName;

  /// The set of top-level modules we have loaded.
  /// This map is used for iteration, therefore it's a MapVector and not a
  /// DenseMap.
  llvm::MapVector<Identifier, ModuleDecl *> loadedModules;

  /// Set if a `-module-alias` was passed. Used to store mapping between module
  /// aliases and their corresponding real names, and vice versa for a reverse
  /// lookup, which is needed to check if the module names appearing in source
  /// files are aliases or real names. \see ASTContext::GetRealModuleName.
  ///
  /// The boolean in the value indicates whether or not the entry is keyed by an
  /// alias vs real name, i.e. true if the entry is [key: alias_name, value:
  /// (real_name, true)].
  mutable llvm::DenseMap<Identifier, std::pair<Identifier, bool>>
      moduleAliasMap;

  /// OutputBackend for writing outputs.
  // std::unique_ptr<llvm::vfs::OutputBackend> outputBackend;

  StatsReporter *stats;

  /// OutputBackend for writing outputs.
  // llvm::IntrusiveRefCntPtr<llvm::vfs::OutputBackend> outputBackend;

public:
  /// The set of cleanups to be called when the ASTContext is destroyed.
  std::vector<std::function<void(void)>> cleanups;

public:
  ASTContext(const ASTContext &) = delete;
  ASTContext &operator=(const ASTContext &) = delete;

  ASTContext(LangOptions &langOpts, const SearchPathOptions &searchPathOpts,
             ClangImporter &clangImporter, DiagnosticEngine &de,
             StatsReporter *stats);

  ~ASTContext();

  /// Add a cleanup function to be called when the ASTContext is deallocated.
  void AddCleanup(std::function<void(void)> cleanup);

public:
  ModuleDecl *mainModule = nullptr;

public:
  ClangImporter &GetClangImporter() { return clangImporter; }
  ///
  Identifier GetIdentifier(llvm::StringRef name);

  DeclNameTable &GetDeclNameTable() { return declNames; }
  ///
  Builtin &GetBuiltin() { return builtin; }

  DiagnosticEngine &GetDiags() { return de; }
  ///
  LangABI *GetLangABI() const;
  //
  SrcMgr &GetSrcMgr() { return de.GetSrcMgr(); }

  LangOptions &GetLangOptions() { return langOpts; }

  StatsReporter *GetStats() { return stats; }
  /// Set a new stats reporter.
  void SetStats(StatsReporter *inputStats) { stats = inputStats; }

public:
  //==Module stuff==//
  // Module *GetModule(UsingPath::Module modulePath);

  /// Attempts to load the matching overlay module for the given clang
  /// module into this ASTContext.
  ///
  /// \returns The Swift overlay module corresponding to the given clang module,
  /// or NULL if the overlay module cannot be found.
  // TODO: Module *GetOverlayModule(const ModuleFile *clangModule);

  ModuleDecl *GetModuleByName(llvm::StringRef moduleName);
  ModuleDecl *GetModuleByIdentifier(Identifier moduleIdentifier);

  /// Returns the standard library module, or null if the library isn't present.
  ///
  /// If \p loadIfAbsent is true, the ASTContext will attempt to load the module
  /// if it hasn't been set yet.
  // Module *GetSTDLibModule(bool loadIfAbsent = false);
  // Module *GetSTDLibModule() const {
  //   return const_cast<ASTContext *>(this)->GetStdlibModule(false);
  // }

  /// Insert an externally-sourced module into the set of known loaded modules
  /// in this context.
  void AddLoadedModule(ModuleDecl *mod);

  /// If \p T is null pointer, assume the target in ASTContext.
  MangleContext *CreateMangleContext(const clang::TargetInfo *T = nullptr);

  Identifier
  GetRealModuleName(Identifier key,
                    ModuleAliasLookupOption option =
                        ModuleAliasLookupOption::AlwaysRealName) const;

  ModuleDecl *GetMainModule() { return mainModule; }
  void SetMainModule(ModuleDecl *inputModule) { mainModule = inputModule; }

  /// Get the output backend. The output backend needs to be initialized via
  /// constructor or `setOutputBackend`.
  // llvm::vfs::OutputBackend &GetOutputBackend() const {
  //   assert(OutputBackend && "OutputBackend is not setup");
  //   return *OutputBackend;
  // }
  // /// Set output backend for virtualized outputs.
  // void SetOutputBackend(
  //     llvm::IntrusiveRefCntPtr<llvm::vfs::OutputBackend> outBackend) {
  //   outputBackend = std::move(outBackend);
  // }
public:
  // THINKING: ASTContext is a friend of the Type so it may be better if it is
  // responsible for adding
  /// and removing qualifiers.

  void AddConstQual(Type *ty);
  void ClearConstQual(Type *ty);

  void AddStoneQual(Type *ty);
  void ClearStoneQual(Type *ty);

public:
  /// Allocate memory from the ASTContext bump pointer.
  void *AllocateMemory(size_t bytes, unsigned alignment = 8) const {
    if (bytes == 0) {
      return nullptr;
    }
    if (langOpts.useMalloc) {
      return stone::AlignedAlloc(bytes, alignment);
    }
    // TODO:
    //  if (arena == MAllocationArena::Stoneanent && Stats)
    //  Stats->GetMemoryCounters().NumMemoryBytesAllocated += bytes;
    return GetAllocator().Allocate(bytes, alignment);
  }

  template <typename T> T *AllocateMemory(size_t num = 1) const {
    return static_cast<T *>(AllocateMemory(num * sizeof(T), alignof(T)));
  }

  ///
  void Deallocate(void *Ptr) const {}

  /// Memory allocator
  llvm::BumpPtrAllocator &GetAllocator() const { return allocator; }

  /// The total amount of memory used
  size_t GetTotalMemoryAllocated() const {
    return GetAllocator().getTotalMemory();
  }

public:
  template <typename T> T *Allocate() const {
    T *res = (T *)AllocateMemory(sizeof(T), alignof(T));
    new (res) T();
    return res;
  }

  template <typename T>
  MutableArrayRef<T> AllocateUninitialized(unsigned NumElts) const {
    T *Data = (T *)AllocateMemory(sizeof(T) * NumElts, alignof(T));
    return {Data, NumElts};
  }

  template <typename T> MutableArrayRef<T> Allocate(unsigned numElts) const {
    T *res = (T *)AllocateMemory(sizeof(T) * numElts, alignof(T));
    for (unsigned i = 0; i != numElts; ++i)
      new (res + i) T();
    return {res, numElts};
  }

  /// Allocate a copy of the specified object.
  template <typename T>
  typename std::remove_reference<T>::type *AllocateObjectCopy(T &&t) const {
    // This function cannot be named AllocateCopy because it would always win
    // overload resolution over the AllocateCopy(ArrayRef<T>).
    using TNoRef = typename std::remove_reference<T>::type;
    TNoRef *res = (TNoRef *)AllocateMemory(sizeof(TNoRef), alignof(TNoRef));
    new (res) TNoRef(std::forward<T>(t));
    return res;
  }

  template <typename T, typename It> T *AllocateCopy(It start, It end) const {
    T *res = (T *)AllocateMemory(sizeof(T) * (end - start), alignof(T));
    for (unsigned i = 0; start != end; ++start, ++i)
      new (res + i) T(*start);
    return res;
  }

  template <typename T, size_t N>
  MutableArrayRef<T> AllocateCopy(T (&array)[N]) const {
    return MutableArrayRef<T>(AllocateCopy<T>(array, array + N), N);
  }

  template <typename T>
  MutableArrayRef<T> AllocateCopy(ArrayRef<T> array) const {
    return MutableArrayRef<T>(AllocateCopy<T>(array.begin(), array.end()),
                              array.size());
  }

  template <typename T>
  ArrayRef<T> AllocateCopy(const SmallVectorImpl<T> &vec) const {
    return AllocateCopy(ArrayRef<T>(vec));
  }

  template <typename T>
  MutableArrayRef<T> AllocateCopy(SmallVectorImpl<T> &vec) const {
    return AllocateCopy(MutableArrayRef<T>(vec));
  }

  StringRef AllocateCopy(StringRef Str) const {
    ArrayRef<char> Result =
        AllocateCopy(llvm::ArrayRef(Str.data(), Str.size()));
    return StringRef(Result.data(), Result.size());
  }

  template <typename T, typename Vector, typename Set>
  MutableArrayRef<T>
  AllocateCopy(llvm::SetVector<T, Vector, Set> setVector) const {
    return MutableArrayRef<T>(
        AllocateCopy<T>(setVector.begin(), setVector.end()), setVector.size());
  }
};
} // namespace stone

#endif