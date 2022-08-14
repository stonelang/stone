#ifndef STONE_BASIC_ERROR_H
#define STONE_BASIC_ERROR_H

#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"

#include <assert.h>
#include <stdlib.h>

namespace stone {

struct alignas(uint8_t) Error final {
private:
  bool err;
  Error &operator=(const Error &other) = delete;

public:
  Error() : err(false) {}
  Error(bool err) : err(err) {}
  bool Has() { return err; }
};

inline void Panic(const char *msg) {
  assert(false && msg);
  (void)msg;
  abort();
}

} // namespace stone
#endif
