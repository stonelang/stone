#include "stone/Syntax/SyntaxContext.h"
#include "stone/Diag/DiagnosticEngine.h"

#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/Support/Allocator.h"
#include "llvm/Support/Compiler.h"

#include <algorithm>
#include <memory>

using namespace stone;
using namespace stone::syn;

struct SyntaxContext::Internal final {
  Internal();
  ~Internal();

  llvm::BumpPtrAllocator allocator;

  using IdentifierTable =
      llvm::StringMap<Identifier::Aligner, llvm::BumpPtrAllocator &>;
  IdentifierTable identifiers;

  /// The set of top-level modules we have loaded.
  /// This map is used for iteration, therefore it's a MapVector and not a
  /// DenseMap.
  // llvm::MapVector<Identifier, ModuleDecl *> loadedModules;

  /// The set of cleanups to be called when the ASTContext is destroyed.
  std::vector<std::function<void(void)>> cleanups;

  struct Arena final {
    // static_assert(alignof(Type) >= 8, "Type not 8-byte aligned?");
  };
  /// The permanent arena.
  Arena permanent;
};

SyntaxContext::Internal::Internal() : identifiers(allocator) {}

SyntaxContext::Internal::~Internal() {}

SyntaxContext::SyntaxContext(stone::LangContext &lc,
                             const SearchPathOptions &spOpts)
    : lc(lc), searchPathOpts(spOpts), identifiers(lc.GetLangOptions()),
      builtin(*this), stats(new SyntaxContextStats(*this)) {

  lc.GetStatEngine().Register(stats.get());
}

SyntaxContext::~SyntaxContext() {
  for (auto &cleanup : cleanups) {
    cleanup();
  }
}

const Builtin &SyntaxContext::GetBuiltin() const { return builtin; }

void *syn::AllocateInSyntaxContext(size_t bytes, const SyntaxContext &ctx,
                                   AllocationArena arena, unsigned alignment) {
  return ctx.Allocate(bytes, alignment /*, arena*/);
}

Identifier &SyntaxContext::GetIdentifier(llvm::StringRef name) {
  return identifiers.Get(name);
}

size_t SyntaxContext::GetSizeOfMemUsed() const {
  return GetAllocator().getTotalMemory();
}

void SyntaxContextStats::Print(ColorfulStream &stream) {}
