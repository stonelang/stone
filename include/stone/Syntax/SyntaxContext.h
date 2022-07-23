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
#include "stone/Basic/SrcMgr.h"
#include "stone/Basic/StatisticEngine.h"
#include "stone/Context.h"
#include "stone/Syntax/Builtin.h"
#include "stone/Syntax/Identifier.h"
#include "stone/Syntax/LangABI.h"
#include "stone/Syntax/SearchPathOptions.h"
#include "stone/Syntax/SyntaxAllocation.h"
#include "stone/Syntax/Type.h"

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
class MangleCtx;
class Module;
class Stmt;
class Builtin;
class SyntaxContext;

class SyntaxContextStats final : public Stats {
  const SyntaxContext &tc;

public:
  SyntaxContextStats(const SyntaxContext &tc)
      : Stats("tree-context stats:"), tc(tc) {}
  void Print(ColorfulStream &stream) override;
};

class SyntaxContext final {
  friend SyntaxContextStats;

  std::unique_ptr<SyntaxContextStats> stats;

  /// The language options used to create the AST associated with
  ///  this SyntaxContext object.
  Context &ctx;

  /// The search path options
  const SearchPathOptions &searchPathOpts;

  Builtin builtin;
  /// The allocator used to create SyntaxContext objects.
  /// SyntaxContext objects are never destructed; rather, all memory associated
  /// with the SyntaxContext objects will be released when the SyntaxContext
  /// itself is destroyed.
  mutable llvm::BumpPtrAllocator bumpAlloc;

  /// Table for all
  IdentifierTable identifiers;
  ///
  mutable llvm::SmallVector<Type *, 0> types;

  /// The standard library module.
  mutable syn::Module *stdlibModule = nullptr;

  /// The name of the standard library module "libstone".
  // Identifier stdlibModuleName;

  struct Detail;
  Detail &GetDetail() const;

public:
  SyntaxContext(const SyntaxContext &) = delete;
  SyntaxContext &operator=(const SyntaxContext &) = delete;

  SyntaxContext(Context &ctx, const SearchPathOptions &spOpts);
  ~SyntaxContext();

  /// Add a cleanup function to be called when the SyntaxContext is deallocated.
  void AddCleanup(std::function<void(void)> cleanup);

public:
  ///
  Identifier &GetIdentifier(llvm::StringRef name);
  ///
  Builtin &GetBuiltin() const;

  Context &GetContext() { return ctx; }
  const Context &GetContext() const { return ctx; }
  ///
  LangABI *GetLangABI() const;
  //
  SrcMgr &GetSrcMgr() { return ctx.GetSrcMgr(); }

  /// Retrieve the allocator for the given arena.
  llvm::BumpPtrAllocator &GetAllocator() const;

  SyntaxContextStats &GetStats() { return *stats.get(); }

public:
  /// Return the total amount of physical memory allocated for representing
  /// AST nodes and type information.
  size_t GetSizeOfMemUsed() const;

  void *Allocate(size_t size, unsigned align = 8) const {
    return bumpAlloc.Allocate(size, align);
  }
  template <typename T> T *Allocate(size_t num = 1) const {
    return static_cast<T *>(Allocate(num * sizeof(T), alignof(T)));
  }
  void Deallocate(void *Ptr) const {}

public:
};
} // namespace syn
} // namespace stone
/// Placement new for using the SyntaxContext's allocator.
///
/// This placement form of operator new uses the SyntaxContext's allocator for
/// obtaining memory.
///
/// IMPORTANT: These are also declared in stone/Syntax/SyntaxContextAllocate.h!
/// Any changes here need to also be made there.
///
/// We intentionally avoid using a nothrow specification here so that the calls
/// to this operator will not perform a null check on the result -- the
/// underlying allocator never returns null pointers.
///
/// Usage looks like this (assuming there's an SyntaxContext 'Context' in
/// scope):
/// @code
/// // Default alignment (8)
/// IntegerLiteral *Ex = new (Context) IntegerLiteral(arguments);
/// // Specific alignment
/// IntegerLiteral *Ex2 = new (Context, 4) IntegerLiteral(arguments);
/// @endcode
/// Memory allocated through this placement new operator does not need to be
/// explicitly freed, as SyntaxContext will free all of this memory when it gets
/// destroyed. Please note that you cannot use delete on the pointer.
///
/// @param Bytes The number of bytes to allocate. Calculated by the sc.
/// @param C The SyntaxContext that provides the allocator.
/// @param Alignment The alignment of the allocated memory (if the underlying
///                  allocator supports it).
/// @return The allocated memory. Could be nullptr.
// inline void *operator new(size_t bytes, const stone::syn::SyntaxContext &tc,
//                           size_t alignment /* = 8 */) {
//   return tc.Allocate(bytes, alignment);
// }

/// Placement delete companion to the new above.
///
/// This operator is just a companion to the new above. There is no way of
/// invoking it directly; see the new operator for more details. This operator
/// is called implicitly by the sc if a placement new expression using
/// the SyntaxContext throws in the object constructor.
// inline void operator delete(void *Ptr, const stone::syn::SyntaxContext &tc,
//                             size_t) {
//   tc.Deallocate(Ptr);
// }

/// This placement form of operator new[] uses the SyntaxContext's allocator for
/// obtaining memory.
///
/// We intentionally avoid using a nothrow specification here so that the calls
/// to this operator will not perform a null check on the result -- the
/// underlying allocator never returns null pointers.
///
/// Usage looks like this (assuming there's an SyntaxContext 'Context' in
/// scope):
/// @code
/// // Default alignment (8)
/// char *data = new (Context) char[10];
/// // Specific alignment
/// char *data = new (Context, 4) char[10];
/// @endcode
/// Memory allocated through this placement new[] operator does not need to be
/// explicitly freed, as SyntaxContext will free all of this memory when it gets
/// destroyed. Please note that you cannot use delete on the pointer.
///
/// @param Bytes The number of bytes to allocate. Calculated by the sc.
/// @param C The SyntaxContext that provides the allocator.
/// @param Alignment The alignment of the allocated memory (if the underlying
///                  allocator supports it).
/// @return The allocated memory. Could be nullptr.
// inline void *operator new[](size_t bytes, const stone::syn::SyntaxContext
// &tc,
//                             size_t alignment /* = 8 */) {
//   return tc.Allocate(bytes, alignment);
// }

/// Placement delete[] companion to the new[] above.
///
/// This operator is just a companion to the new[] above. There is no way of
/// invoking it directly; see the new[] operator for more details. This operator
/// is called implicitly by the sc if a placement new[] expression using
/// the SyntaxContext throws in the object constructor.
// inline void operator delete[](void *Ptr, const stone::syn::SyntaxContext &tc,
//                               size_t) {
//   tc.Deallocate(Ptr);
// }

#endif
