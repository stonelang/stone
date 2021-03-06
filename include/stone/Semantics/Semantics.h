#ifndef STONE_SEMANTICS_SEMANTICS_H
#define STONE_SEMANTICS_SEMANTICS_H

#include "stone/Semantics/TypeChecker.h"

#include "llvm/ADT/ArrayRef.h"

namespace stone {
namespace sema {

class Semantics final {
  std::unique_ptr<TypeChecker> checker;

public:
  Semantics();
  ~Semantics();

public:
  TypeChecker &GetChecker() { return *checker.get(); }
};

} // namespace sema

} // namespace stone
#endif
