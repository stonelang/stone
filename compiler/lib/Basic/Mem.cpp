#include "stone/Basic/Mem.h"

using namespace stone;

void *stone::AllocateInMemoryContext(size_t bytes,
                                     const MemoryContext &memContext,
                                     MemoryArena arena, unsigned alignment) {
  return memContext.Allocate(bytes, alignment /*, arena*/);
}

MemoryContext::MemoryContext(const LangOptions &langOpts)
    : langOpts(langOpts) {}