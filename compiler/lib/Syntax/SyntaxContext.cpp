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

// SyntaxContext::Internal::Internal() : identifiers(allocator) {}

// SyntaxContext::Internal::~Internal() {}

SyntaxContext::SyntaxContext(stone::LangContext &lc,
                             const SearchPathOptions &spOpts)
    : lc(lc), searchPathOpts(spOpts), identifiers(allocator), builtin(*this),
      stats(new SyntaxContextStats(*this)) {

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

Identifier SyntaxContext::GetIdentifier(llvm::StringRef name) {
  return identifiers.GetIdentifier(name);
}

size_t SyntaxContext::GetSizeOfMemUsed() const {
  return GetAllocator().getTotalMemory();
}

// llvm::BumpPtrAllocator &SyntaxContext::GetBumpAllocator() const {
//   return internal.allocator;
// }

FunctionType::FunctionType(const SyntaxContext *sc, QualType result)
    : AbstractFunctionType(TypeKind::Function, sc, result) {}

void SyntaxContextStats::Print(ColorfulStream &stream) {}
