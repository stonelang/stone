#include "stone/Basic/Memory.h"

using namespace stone;

void *stone::AllocateInMemoryContext(size_t bytes, const MemoryContext &mem,
                                     MemoryAllocationArena arena,
                                     unsigned alignment) {
  return mem.AllocateMemory(bytes, alignment /*, arena*/);
}

MemoryContext::MemoryContext(const LangOptions &langOpts)
    : langOpts(langOpts) {}