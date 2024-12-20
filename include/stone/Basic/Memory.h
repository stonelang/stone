#ifndef STONE_BASIC_MEMORY_H
#define STONE_BASIC_MEMORY_H

#include "stone/Basic/LangOptions.h"

#include "llvm/Support/Allocator.h"

#include <cassert>
#include <cstring>
#include <memory>

#if defined(_WIN32)
#include <malloc.h>
#else
#include <cstdlib>
#endif

namespace stone {

/// The arena in which a particular allocation object will go.
enum class MemoryAllocationArena {
  /// The stoneanent arena, which is tied to the lifetime of
  /// the object
  ///
  /// All global declarations and types need to be allocated into this arena.
  /// At present, everything that is not a type involving a type variable is
  /// allocated in this arena.
  Stoneanent,
  /// The constraint solver's temporary arena, which is tied to the
  /// lifetime of a particular instance of the constraint solver.
  ///
  /// Any type involving a type variable is allocated in this arena.
  Temporary
};

// FIXME: Use C11 aligned_alloc if available.
inline void *AlignedAlloc(size_t size, size_t align) {
  // posix_memalign only accepts alignments greater than sizeof(void*).
  //
  if (align < sizeof(void *))
    align = sizeof(void *);

  void *r;
#if defined(_WIN32)
  r = _aligned_malloc(size, align);
  assert(r && "_aligned_malloc failed");
#else
  int res = posix_memalign(&r, align, size);
  assert(res == 0 && "posix_memalign failed");
  (void)res; // Silence the unused variable warning.
#endif
  return r;
}

inline void AlignedFree(void *p) {
#if defined(_WIN32)
  _aligned_free(p);
#else
  free(p);
#endif
}

inline void *Copy(void *dest, const void *src, std::size_t count) {
  return std::memcpy(dest, src, count);
}

class MemoryContext {
protected:
  const LangOptions &langOpts;
  mutable llvm::BumpPtrAllocator allocator;

public:
  MemoryContext(const MemoryContext &) = delete;
  MemoryContext &operator=(const MemoryContext &) = delete;

public:
  MemoryContext(const LangOptions &langOpts);

public:
  /// Allocate - Allocate memory from the ASTContext bump pointer.
  void *AllocateMemory(
      unsigned long bytes, unsigned alignment = 8,
      MemoryAllocationArena arena = MemoryAllocationArena::Stoneanent) const {
    if (bytes == 0) {
      return nullptr;
    }
    if (langOpts.useMalloc) {
      return stone::AlignedAlloc(bytes, alignment);
    }
    // TODO:
    //  if (arena == MemoryAllocationArena::Stoneanent && Stats)
    //  Stats->GetMemoryCounters().NumMemoryBytesAllocated += bytes;
    return GetAllocator(arena).Allocate(bytes, alignment);
  }

  void Deallocate(void *Ptr) const {}

public:
  llvm::BumpPtrAllocator &GetAllocator(
      MemoryAllocationArena arena = MemoryAllocationArena::Stoneanent) const {
    return allocator;
  }

  size_t GetTotalMemUsed() const { return GetAllocator().getTotalMemory(); }
};

void *AllocateInMemoryContext(size_t bytes, const MemoryContext &ctx,
                              MemoryAllocationArena arena, unsigned alignment);

/// Types inheriting from this class are intended to be allocated in an
/// \c ASTContext allocator; you cannot allocate them by using a normal \c
/// new, and instead you must either provide an \c ASTContext or use a placement
/// \c new.
///
/// The template parameter is a type with the desired alignment. It is usually,
/// but not always, the type that is inheriting \c ASTAllocated.
template <typename AlignTy> class MemoryAllocation {
public:
  // Make vanilla new/delete illegal.
  void *operator new(size_t bytes) throw() = delete;
  void operator delete(void *data) throw() = delete;

  // Only allow allocation using the allocator in MemoryContext
  // or by doing a placement new.
  void *
  operator new(size_t bytes, const MemoryContext &ctx,
               MemoryAllocationArena arena = MemoryAllocationArena::Stoneanent,
               unsigned alignment = alignof(AlignTy)) {
    return stone::AllocateInMemoryContext(bytes, ctx, arena, alignment);
  }
  void *operator new(size_t bytes, void *mem) throw() {
    assert(mem && "placement new into failed allocation");
    return mem;
  }
};

} // namespace stone

#endif