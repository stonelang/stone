#ifndef STONE_BASIC_MEM_H
#define STONE_BASIC_MEM_H

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
enum class AllocationArena {
  /// The permanent arena, which is tied to the lifetime of
  /// the object
  ///
  /// All global declarations and types need to be allocated into this arena.
  /// At present, everything that is not a type involving a type variable is
  /// allocated in this arena.
  Permanent,
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

// template <typename T> class MemContext {
//   llvm::StringMap<T *, llvm::BumpPtrAllocator> entries;
// public:
//   llvm::BumpPtrAllocator &GetAllocator() { return entries.getAllocator(); }
//   /// Return the identifier token info for the specified named
//   /// identifier.
//   T &Get(T ty) {
//     auto &entry = *entries.insert(std::make_pair(ty, nullptr)).first;
//     T *&second = entry.second;
//     if (second) {
//       return *second;
//     }
//     // Lookups failed, make a new Identifier.
//     void *mem = GetAllocator().template Allocate<T>();
//     second = new (mem) T();
//     return *second;
//   }

// enum class MemArena { Permanent = 0, Temporary };
// template <typename AlignTy, typename ContextTy> class MemAllocation {
// public:
//   // Make vanilla new/delete illegal.
//   void *operator new(size_t bytes) throw() = delete;
//   void operator delete(void *data) throw() = delete;

//   // Only allow allocation using the allocator in ASTContext
//   // or by doing a placement new.
//   void *operator new(size_t bytes, const ContextTy &ctx,
//                      llvm::function_ref <
//                          void *(size_t bytes, const ContextTy &ctx,
//                                 MemArena arena,
//                                 unsigned alignment)PerformAllocation,
//                      MemArena arena = MemArena::Permanent,
//                      unsigned alignment = alignof(AlignTy)) {
//     return PerformAllocation(bytes, ctx, arena, alignment);
//   }

//   void *operator new(size_t bytes, void *mem) throw() {
//     assert(mem && "placement new into failed allocation");
//     return mem;
//   }
// };
} // namespace stone

#endif
