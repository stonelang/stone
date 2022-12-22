#ifndef STONE_CLANG_CLANGAST_H
#define STONE_CLANG_CLANGAST_H

#include "llvm/ADT/ArrayRef.h"

namespace stone {

template <typename T> struct ClangASTNodeBox {
  const T *const value;

  ClangASTNodeBox() : value{nullptr} {}
  /*implicit*/ ClangASTNodeBox(const T *V) : value(V) {}

  explicit operator bool() const { return value; }
};

class ClangASTNode final {
public:
  ClangASTNode();
};

} // namespace stone
#endif
