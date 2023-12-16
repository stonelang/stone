#ifndef STONE_SYNTAX_ASTCONTEXT_H
#define STONE_SYNTAX_ASTCONTEXT_H

#include "stone/Basic/LangOptions.h"
#include "stone/Basic/Mem.h"
#include "stone/Basic/SrcMgr.h"
#include "stone/Basic/StatisticEngine.h"
#include "stone/Public.h"
#include "stone/Syntax/ASTAllocation.h"
#include "stone/Syntax/BuiltinContext.h"
#include "stone/Syntax/DeclName.h"
#include "stone/Syntax/Identifier.h"
#include "stone/Syntax/Import.h"
#include "stone/Syntax/LangABI.h"
#include "stone/Syntax/SearchPath.h"
#include "stone/Syntax/Types.h"

#include "stone/Basic/SrcLoc.h"
#include "stone/Diag/DiagnosticEngine.h"
#include "stone/Syntax/ASTDiagnosticArgument.h"
#include "stone/Syntax/ClangContext.h"
#include "stone/Syntax/Expr.h"
#include "stone/Syntax/Ownership.h"
#include "stone/Syntax/Specifier.h"
#include "stone/Syntax/SyntaxResult.h"
#include "stone/Syntax/Types.h"

#include "llvm/ADT/APSInt.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/FoldingSet.h"
#include "llvm/ADT/IntrusiveRefCntPtr.h"
#include "llvm/ADT/MapVector.h"
#include "llvm/ADT/None.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/PointerUnion.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/TinyPtrVector.h"
#include "llvm/ADT/Triple.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/Support/AlignOf.h"
#include "llvm/Support/Allocator.h"
#include "llvm/Support/Casting.h"

// UPDATE #include "llvm/Support/VirtualOutputBackend.h"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <memory>
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
class BuiltinContext;
class ASTContext;
class Decl;
class DeclContext;
class FunDecl;
class StructDecl;
class Stmt;
class IfStmt;
class SwitchStmt;
class Expr;
class SourceFile;
class CompilerStatsReporter;

class ASTContextStats final : public Stats {
  const ASTContext &sc;

public:
  ASTContextStats(const ASTContext &sc)
      : Stats("syntax context stats:"), sc(sc) {}
  void Print(ColorStream &stream) override;
};

/// Look up option used in \c GetRealModuleName when module aliasing is applied.
enum class ModuleAliasLookupOption {
  AlwaysRealName,
  RealNameFromAlias,
  AliasFromRealName
};
class ASTContext final {
  friend ASTContextStats;

  std::unique_ptr<ASTContextStats> stats;

  ClangContext &clangContext;

  /// The search path options
  const SearchPathOptions &searchPathOpts;

  DiagnosticEngine &de;

  StatisticEngine &se;

  LangOptions &langOpts;

  BuiltinContext builtinContext;
  /// The allocator used to create ASTContext objects.
  /// ASTContext objects are never destructed; rather, all memory associated
  /// with the ASTContext objects will be released when the ASTContext
  /// itself is destroyed.
  mutable llvm::BumpPtrAllocator allocator;

  /// Table for all
  IdentifierTable identifiers;

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

  CompilerStatsReporter *statsReporter;

public:
  /// The set of cleanups to be called when the ASTContext is destroyed.
  std::vector<std::function<void(void)>> cleanups;

public:
  ASTContext(const ASTContext &) = delete;
  ASTContext &operator=(const ASTContext &) = delete;

  ASTContext(LangOptions &langOpts, const SearchPathOptions &searchPathOpts,
             ClangContext &clangContext, DiagnosticEngine &de,
             StatisticEngine &se);

  ~ASTContext();

  /// Add a cleanup function to be called when the ASTContext is deallocated.
  void AddCleanup(std::function<void(void)> cleanup);

public:
  ClangContext &GetClangContext() { return clangContext; }
  ///
  Identifier GetIdentifier(llvm::StringRef name);

  DeclNameTable &GetDeclNameTable() { return declNames; }
  ///
  const BuiltinContext &GetBuiltinContext() const;
  DiagnosticEngine &GetDiags() { return de; }
  ///
  LangABI *GetLangABI() const;
  //
  SrcMgr &GetSrcMgr() { return de.GetSrcMgr(); }

  LangOptions &GetLangOptions() { return langOpts; }

  ModuleDecl *mainModule = nullptr;

  /// Retrieve the allocator for the given arena.
  /// Using permanent always for now
  llvm::BumpPtrAllocator &
  GetAllocator(AllocationArena arena = AllocationArena::Permanent) const {
    return allocator;
  }
  StatisticEngine &GetStats() { return se; }
  ASTContextStats &GetASTContextStats() { return *stats; }

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

  /// Set a new stats reporter.
  void SetStatsReporter(CompilerStatsReporter *stats) { statsReporter = stats; }

public:
  stone::InFlightDiagnostic PrintD(SrcLoc loc, DiagID diagID) {
    return GetDiags().PrintD(
        loc, ASTDiagnostic(diagID, llvm::ArrayRef<diag::Argument>()));
  }
  stone::InFlightDiagnostic PrintD(SrcLoc loc, DiagID diagID,
                                   llvm::ArrayRef<diag::Argument> args) {
    return GetDiags().PrintD(loc, ASTDiagnostic(diagID, args));
  }

  template <typename... ArgTypes>
  stone::InFlightDiagnostic
  PrintD(SrcLoc loc, Diag<ArgTypes...> id,
         typename stone::detail::PassArgument<ArgTypes>::type... args) {
    return GetDiags().PrintD(loc, ASTDiagnostic(id, std::move(args)...));
  }

public:
  /// Return the total amount of physical memory allocated for representing
  /// AST nodes and type information.
  size_t GetTotalMemUsed() const;
  void Deallocate(void *Ptr) const {}

  /// Allocate - Allocate memory from the ASTContext bump pointer.
  void *Allocate(unsigned long bytes, unsigned alignment = 8,
                 AllocationArena arena = AllocationArena::Permanent) const {
    if (bytes == 0) {
      return nullptr;
    }
    if (langOpts.useMalloc) {
      return stone::AlignedAlloc(bytes, alignment);
    }
    // TODO:
    //  if (arena == AllocationArena::Permanent && Stats)
    //    Stats->GetFrontendCounters().NumASTBytesAllocated += bytes;

    return GetAllocator(arena).Allocate(bytes, alignment);
  }

public:
  template <typename T>
  T *Allocate(AllocationArena arena = AllocationArena::Permanent) const {
    T *res = (T *)Allocate(sizeof(T), alignof(T), arena);
    new (res) T();
    return res;
  }

  template <typename T>
  MutableArrayRef<T> AllocateUninitialized(
      unsigned NumElts,
      AllocationArena Arena = AllocationArena::Permanent) const {
    T *Data = (T *)Allocate(sizeof(T) * NumElts, alignof(T), Arena);
    return {Data, NumElts};
  }

  template <typename T>
  MutableArrayRef<T>
  Allocate(unsigned numElts,
           AllocationArena arena = AllocationArena::Permanent) const {
    T *res = (T *)Allocate(sizeof(T) * numElts, alignof(T), arena);
    for (unsigned i = 0; i != numElts; ++i)
      new (res + i) T();
    return {res, numElts};
  }

  /// Allocate a copy of the specified object.
  template <typename T>
  typename std::remove_reference<T>::type *
  AllocateObjectCopy(T &&t,
                     AllocationArena arena = AllocationArena::Permanent) const {
    // This function cannot be named AllocateCopy because it would always win
    // overload resolution over the AllocateCopy(ArrayRef<T>).
    using TNoRef = typename std::remove_reference<T>::type;
    TNoRef *res = (TNoRef *)Allocate(sizeof(TNoRef), alignof(TNoRef), arena);
    new (res) TNoRef(std::forward<T>(t));
    return res;
  }

  template <typename T, typename It>
  T *AllocateCopy(It start, It end,
                  AllocationArena arena = AllocationArena::Permanent) const {
    T *res = (T *)Allocate(sizeof(T) * (end - start), alignof(T), arena);
    for (unsigned i = 0; start != end; ++start, ++i)
      new (res + i) T(*start);
    return res;
  }

  template <typename T, size_t N>
  MutableArrayRef<T>
  AllocateCopy(T (&array)[N],
               AllocationArena arena = AllocationArena::Permanent) const {
    return MutableArrayRef<T>(AllocateCopy<T>(array, array + N, arena), N);
  }

  template <typename T>
  MutableArrayRef<T>
  AllocateCopy(ArrayRef<T> array,
               AllocationArena arena = AllocationArena::Permanent) const {
    return MutableArrayRef<T>(
        AllocateCopy<T>(array.begin(), array.end(), arena), array.size());
  }

  template <typename T>
  ArrayRef<T>
  AllocateCopy(const SmallVectorImpl<T> &vec,
               AllocationArena arena = AllocationArena::Permanent) const {
    return AllocateCopy(ArrayRef<T>(vec), arena);
  }

  template <typename T>
  MutableArrayRef<T>
  AllocateCopy(SmallVectorImpl<T> &vec,
               AllocationArena arena = AllocationArena::Permanent) const {
    return AllocateCopy(MutableArrayRef<T>(vec), arena);
  }

  StringRef
  AllocateCopy(StringRef Str,
               AllocationArena arena = AllocationArena::Permanent) const {
    ArrayRef<char> Result =
        AllocateCopy(llvm::makeArrayRef(Str.data(), Str.size()), arena);
    return StringRef(Result.data(), Result.size());
  }

  template <typename T, typename Vector, typename Set>
  MutableArrayRef<T>
  AllocateCopy(llvm::SetVector<T, Vector, Set> setVector,
               AllocationArena arena = AllocationArena::Permanent) const {
    return MutableArrayRef<T>(
        AllocateCopy<T>(setVector.begin(), setVector.end(), arena),
        setVector.size());
  }
};
} // namespace stone

#endif
