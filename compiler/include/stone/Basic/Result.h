#ifndef STONE_BASIC_RESULT_H
#define STONE_BASIC_RESULT_H

#include "stone/Basic/Error.h"

#include <memory>

namespace stone {

template <typename T> class Result final {
  T ty;
  Error err;

public:
  Result() : err(true) {}
  Result(T ty) : ty(ty), err(false) {}

public:
  Error &GetError() { return err; }
  T &GetRef() const { return *ty; }
  template <typename A> A *GetAs() { return static_cast<A *>(ty.get()); }
};

template <typename T> class SafeResult final {
  Error err;
  std::unique_ptr<T> ty;

public:
  SafeResult() : err(true) {}
  SafeResult(std::unique_ptr<T> ty) : ty(std::move(ty)), err(false) {}

public:
  Error &GetError() { return err; }
  T &GetRef() const { return *ty; }
  template <typename A> A *GetAs() { return static_cast<A *>(ty.get()); }
};

} // namespace stone
#endif
