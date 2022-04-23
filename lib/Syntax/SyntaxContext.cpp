#include "stone/Syntax/SyntaxContext.h"

#include <algorithm>
#include <memory>

#include "stone/Basic/DiagnosticEngine.h"
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

SyntaxContext::~SyntaxContext() {}

struct SyntaxContext::Detail final {
  Detail();
  ~Detail();

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

// TODO:
inline SyntaxContext::Detail &SyntaxContext::GetDetail() const {
  auto pointer = reinterpret_cast<char *>(const_cast<SyntaxContext *>(this));
  auto offset =
      llvm::alignAddr((void *)sizeof(*this), llvm::Align(alignof(Detail)));
  return *reinterpret_cast<Detail *>(pointer + offset);
}

Identifier &SyntaxContext::GetIdentifier(llvm::StringRef name) {
  return identifiers.Get(name);
}

size_t SyntaxContext::GetSizeOfMemUsed() const {
  // TODO: use ctx.GetBumpAlloc()
  return bumpAlloc.getTotalMemory();
}

void SyntaxContextStats::Print(ColorfulStream &stream) {}
