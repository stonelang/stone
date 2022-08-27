#include "stone/Syntax/SyntaxContext.h"

#include <algorithm>
#include <memory>

#include "stone/Diag/DiagnosticEngine.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/Support/Allocator.h"
#include "llvm/Support/Compiler.h"

using namespace stone;
using namespace stone::syn;

struct SyntaxContext::Extension final {
  Extension();
  ~Extension();

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
    static_assert(alignof(Type) >= 8, "Type not 8-byte aligned?");
  };
  /// The permanent arena.
  Arena permanent;
};

SyntaxContext::Extension::Extension() : identifiers(allocator) {}

SyntaxContext::Extension::~Extension() {}

SyntaxContext::SyntaxContext(stone::LangContext &ctx,
                             const SearchPathOptions &spOpts)
    : ctx(ctx), searchPathOpts(spOpts), identifiers(ctx.GetLangOptions()) {
  stats = std::make_unique<SyntaxContextStats>(*this);
  ctx.GetStatEngine().Register(stats.get());

  builtin.Init(*this);
}

SyntaxContext::~SyntaxContext() {
  for (auto &cleanup : cleanups) {
    cleanup();
  }
}

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
