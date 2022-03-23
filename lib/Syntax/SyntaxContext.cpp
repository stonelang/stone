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

class SyntaxContext::Detail {

public:
  Detail();
  ~Detail();

public:
  llvm::BumpPtrAllocator Allocator; // used in later initializations

  /// The set of cleanups to be called when the SyntaxContext is destroyed.
  std::vector<std::function<void(void)>> cleanups;

  /// The set of top-level modules we have loaded.
  /// This map is used for iteration, therefore it's a MapVector and not a
  /// DenseMap.
  llvm::MapVector<Identifier *, syn::Module *> loadedModules;
};

SyntaxContext::Detail::Detail() {}

SyntaxContext::Detail::~Detail() {
  for (auto &cleanup : cleanups) {
    cleanup();
  }
}

Identifier &SyntaxContext::GetIdentifier(llvm::StringRef name) {
  return identifiers.Get(name);
}
size_t SyntaxContext::GetSizeOfMemUsed() const {
  // TODO: use ctx.GetBumpAlloc()
  return bumpAlloc.getTotalMemory();
}

void SyntaxContextStats::Print() {}
