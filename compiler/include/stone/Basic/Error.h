#ifndef STONE_BASIC_ERROR_H
#define STONE_BASIC_ERROR_H

#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"

#include <assert.h>
#include <stdlib.h>

namespace stone {

constexpr size_t ErrorAlignment = 8;

struct alignas(1 << ErrorAlignment) Error final {
private:
  bool err;
  Error &operator=(const Error &other) = delete;

public:
  Error() : err(false) {}
  Error(bool err) : err(err) {}
  bool Has() { return err; }
};

inline void Panic(const char *msg) { llvm_unreachable(msg); }

} // namespace stone
#endif
