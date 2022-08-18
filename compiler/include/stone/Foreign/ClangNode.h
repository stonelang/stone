#ifndef STONE_CLANG_CLANNODE_H
#define STONE_CLANG_CLANNODE_H

#include "llvm/ADT/ArrayRef.h"

namespace stone {

template <typename T> struct ClangNodeBox {
  const T *const value;

  ClangNodeBox() : value{nullptr} {}
  /*implicit*/ ClangNodeBox(const T *V) : value(V) {}

  explicit operator bool() const { return value; }
};

class ClangNode final {
public:
  ClangNode();
};

} // namespace stone
#endif
