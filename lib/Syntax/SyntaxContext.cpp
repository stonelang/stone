#include "stone/Syntax/SyntaxContext.h"

#include <algorithm>
#include <memory>

#include "stone/Core/DiagnosticEngine.h"
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
    : ctx(ctx), searchPathOpts(spOpts), identifiers(ctx.GetSystemOptions()) {
  stats = std::make_unique<SyntaxContextStats>(*this, ctx);
  ctx.GetStatEngine().Register(stats.get());

  builtin.Init(*this);
}

SyntaxContext::~SyntaxContext() {}

Identifier &SyntaxContext::GetIdentifier(llvm::StringRef name) {
  return identifiers.Get(name);
}
size_t SyntaxContext::GetSizeOfMemUsed() const {
  // TODO: use ctx.GetBumpAlloc()
  return bumpAlloc.getTotalMemory();
}

void SyntaxContextStats::Print() {}
