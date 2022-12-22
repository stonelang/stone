#ifndef STONE_SYNTAX_TREECONTEXT_H
#define STONE_SYNTAX_TREECONTEXT_H

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include "stone/Basic/LangOptions.h"
#include "stone/Basic/Mem.h"
#include "stone/Basic/SrcMgr.h"
#include "stone/Basic/StatisticEngine.h"
#include "stone/Public.h"
#include "stone/Syntax/BuiltinContext.h"
#include "stone/Syntax/Identifier.h"
#include "stone/Syntax/Import.h"
#include "stone/Syntax/LangABI.h"
#include "stone/Syntax/SearchPath.h"
#include "stone/Syntax/SyntaxAllocation.h"
#include "stone/Syntax/Types.h"

#include "stone/Basic/SrcLoc.h"
#include "stone/Diag/DiagnosticEngine.h"
#include "stone/Foreign/ClangContext.h"
#include "stone/Syntax/Expr.h"
#include "stone/Syntax/Ownership.h"
#include "stone/Syntax/Specifier.h"
#include "stone/Syntax/SyntaxDiagnosticArgument.h"
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

namespace stone {
class DiagnosticEngine;

namespace syn {

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
class SyntaxContext;
class Decl;
class DeclContext;
class FunDecl;
class StructDecl;
class Stmt;
class IfStmt;
class SwitchStmt;
class Expr;
class SyntaxFile;

class SyntaxContextStats final : public Stats {
  const SyntaxContext &sc;

public:
  SyntaxContextStats(const SyntaxContext &sc)
      : Stats("syntax context stats:"), sc(sc) {}
  void Print(ColorfulStream &stream) override;
};

/// Look up option used in \c GetRealModuleName when module aliasing is applied.
enum class ModuleAliasLookupOption {
  AlwaysRealName,
  RealNameFromAlias,
  AliasFromRealName
};
class SyntaxContext final {
  friend SyntaxContextStats;

  mem::Safe<SyntaxContextStats> stats;

  /// The language options used to create the AST associated with
  ///  this SyntaxContext object.
  LangContext &lc;

  ClangContext &clangContext;

  /// The search path options
  const SearchPathOptions &searchPathOpts;

  BuiltinContext builtinContext;
  /// The allocator used to create SyntaxContext objects.
  /// SyntaxContext objects are never destructed; rather, all memory associated
  /// with the SyntaxContext objects will be released when the SyntaxContext
  /// itself is destroyed.
  mutable llvm::BumpPtrAllocator allocator;

  /// Table for all
  IdentifierTable identifiers;

  /// All builtin types will be stored here.
  mutable llvm::SmallVector<Type *, 0> types;

  /// The standard library module.
  mutable syn::ModuleDecl *stdlibModule = nullptr;

  /// The name of the standard library module "libstone".
  // Identifier stdlibModuleName;

  /// The set of top-level modules we have loaded.
  /// This map is used for iteration, therefore it's a MapVector and not a
  /// DenseMap.
  llvm::MapVector<Identifier, syn::ModuleDecl *> loadedModules;

  /// Set if a `-module-alias` was passed. Used to store mapping between module
  /// aliases and their corresponding real names, and vice versa for a reverse
  /// lookup, which is needed to check if the module names appearing in source
  /// files are aliases or real names. \see SyntaxContext::GetRealModuleName.
  ///
  /// The boolean in the value indicates whether or not the entry is keyed by an
  /// alias vs real name, i.e. true if the entry is [key: alias_name, value:
  /// (real_name, true)].
  mutable llvm::DenseMap<Identifier, std::pair<Identifier, bool>>
      moduleAliasMap;

public:
  /// The set of cleanups to be called when the SyntaxContext is destroyed.
  std::vector<std::function<void(void)>> cleanups;

public:
  SyntaxContext(const SyntaxContext &) = delete;
  SyntaxContext &operator=(const SyntaxContext &) = delete;

  SyntaxContext(LangContext &lc, const SearchPathOptions &searchPathOpts,
                ClangContext &clangContext);
  ~SyntaxContext();

  /// Add a cleanup function to be called when the SyntaxContext is deallocated.
  void AddCleanup(std::function<void(void)> cleanup);

public:
  ClangContext &GetClangContext() { return clangContext; }
  ///
  Identifier GetIdentifier(llvm::StringRef name);
  ///
  const BuiltinContext &GetBuiltinContext() const;

  LangContext &GetLangContext() { return lc; }
  const LangContext &GetLangContext() const { return lc; }
  ///
  LangABI *GetLangABI() const;
  //
  SrcMgr &GetSrcMgr() { return lc.GetSrcMgr(); }

  /// Retrieve the allocator for the given arena.
  llvm::BumpPtrAllocator &GetAllocator() const { return allocator; }
  SyntaxContextStats &GetStats() { return *stats.get(); }

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
  //   return const_cast<SyntaxContext *>(this)->GetStdlibModule(false);
  // }

  /// Insert an externally-sourced module into the set of known loaded modules
  /// in this context.
  void AddLoadedModule(ModuleDecl *mod);

  Identifier
  GetRealModuleName(Identifier key,
                    ModuleAliasLookupOption option =
                        ModuleAliasLookupOption::AlwaysRealName) const;

private:
public:
  /// Return the total amount of physical memory allocated for representing
  /// AST nodes and type information.
  size_t GetSizeOfMemUsed() const;

  void *Allocate(size_t size, unsigned align = 8) const {
    return GetAllocator().Allocate(size, align);
  }
  template <typename T> T *Allocate(size_t num = 1) const {
    return static_cast<T *>(Allocate(num * sizeof(T), alignof(T)));
  }
  void Deallocate(void *Ptr) const {}

public:
  stone::InFlightDiagnostic PrintD(SrcLoc loc, DiagID diagID) {
    return GetLangContext().GetDiagUnit().PrintD(
        loc, SyntaxDiagnostic(
                 DiagnosticDetail(diagID, llvm::ArrayRef<diag::Argument>())));
  }
  stone::InFlightDiagnostic PrintD(SrcLoc loc, DiagID diagID,
                                   llvm::ArrayRef<diag::Argument> args) {
    return GetLangContext().GetDiagUnit().PrintD(
        loc, SyntaxDiagnostic(DiagnosticDetail(diagID, args)));
  }

  template <typename... ArgTypes>
  stone::InFlightDiagnostic
  PrintD(SrcLoc loc, Diag<ArgTypes...> id,
         typename stone::detail::PassArgument<ArgTypes>::type... args) {
    return GetLangContext().GetDiagUnit().PrintD(
        loc, SyntaxDiagnostic(DiagnosticDetail(id, std::move(args)...)));
  }
};
} // namespace syn
} // namespace stone

#endif
