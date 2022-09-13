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
