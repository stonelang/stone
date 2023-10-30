#ifndef STONE_AST_TREECONTEXT_H
#define STONE_AST_TREECONTEXT_H

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
#include "stone/AST/Builtin.h"
#include "stone/AST/DeclName.h"
#include "stone/AST/Identifier.h"
#include "stone/AST/Import.h"
#include "stone/AST/LangABI.h"
#include "stone/AST/SearchPath.h"
#include "stone/AST/ASTAllocation.h"
#include "stone/AST/Types.h"

#include "stone/Basic/SrcLoc.h"
#include "stone/Diag/DiagnosticEngine.h"
#include "stone/AST/ClangContext.h"
#include "stone/AST/Expr.h"
#include "stone/AST/Ownership.h"
#include "stone/AST/Specifier.h"
#include "stone/AST/ASTDiagnosticArgument.h"
#include "stone/AST/ASTResult.h"
#include "stone/AST/Types.h"

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

namespace ast {

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
class ASTFile;

class ASTContextStats final : public Stats {
  const ASTContext &sc;

public:
  ASTContextStats(const ASTContext &sc)
      : Stats("asttax context stats:"), sc(sc) {}
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

  Safe<ASTContextStats> stats;

  /// The language options used to create the AST associated with
  ///  this ASTContext object.
  LangContext &lc;

  ClangContext &clangContext;

  /// The search path options
  const SearchPathOptions &searchPathOpts;

  Builtin builtinContext;
  /// The allocator used to create ASTContext objects.
  /// ASTContext objects are never destructed; rather, all memory associated
  /// with the ASTContext objects will be released when the ASTContext
  /// itself is destroyed.
  mutable llvm::BumpPtrAllocator allocator;

  /// Table for all
  IdentifierTable identifiers;

  mutable DeclNameTable declNames;

  /// All builtin types will be stored here.
  mutable llvm::SmallVector<Type *, 0> types;

  /// The standard library module.
  mutable ast::ModuleDecl *stdlibModule = nullptr;

  /// The name of the standard library module "libstone".
  // Identifier stdlibModuleName;

  /// The set of top-level modules we have loaded.
  /// This map is used for iteration, therefore it's a MapVector and not a
  /// DenseMap.
  llvm::MapVector<Identifier, ast::ModuleDecl *> loadedModules;

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

public:
  /// The set of cleanups to be called when the ASTContext is destroyed.
  std::vector<std::function<void(void)>> cleanups;

public:
  ASTContext(const ASTContext &) = delete;
  ASTContext &operator=(const ASTContext &) = delete;

  ASTContext(LangContext &lc, const SearchPathOptions &searchPathOpts,
                ClangContext &clangContext);
  ~ASTContext();

  /// Add a cleanup function to be called when the ASTContext is deallocated.
  void AddCleanup(std::function<void(void)> cleanup);

public:
  ClangContext &GetClangContext() { return clangContext; }
  ///
  Identifier GetIdentifier(llvm::StringRef name);

  DeclNameTable &GetDeclNameTable() { return declNames; }
  ///
  const Builtin &GetBuiltin() const;

  LangContext &GetLangContext() { return lc; }
  const LangContext &GetLangContext() const { return lc; }
  ///
  LangABI *GetLangABI() const;
  //
  SrcMgr &GetSrcMgr() { return lc.GetSrcMgr(); }

  /// Retrieve the allocator for the given arena.
  llvm::BumpPtrAllocator &GetAllocator() const { return allocator; }
  ASTContextStats &GetStats() { return *stats.get(); }

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
        loc, ASTDiagnostic(
                 DiagnosticDetail(diagID, llvm::ArrayRef<diag::Argument>())));
  }
  stone::InFlightDiagnostic PrintD(SrcLoc loc, DiagID diagID,
                                   llvm::ArrayRef<diag::Argument> args) {
    return GetLangContext().GetDiagUnit().PrintD(
        loc, ASTDiagnostic(DiagnosticDetail(diagID, args)));
  }

  template <typename... ArgTypes>
  stone::InFlightDiagnostic
  PrintD(SrcLoc loc, Diag<ArgTypes...> id,
         typename stone::detail::PassArgument<ArgTypes>::type... args) {
    return GetLangContext().GetDiagUnit().PrintD(
        loc, ASTDiagnostic(DiagnosticDetail(id, std::move(args)...)));
  }
};
} // namespace ast
} // namespace stone

#endif
