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

SyntaxContext::SyntaxContext(stone::Context &ctx,
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
