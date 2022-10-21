#include "stone/Syntax/Type.h"

#ifndef STONE_SYNTAX_CANTYPE_H
#define STONE_SYNTAX_CANTYPE_H

namespace stone {

namespace syn {

class CanType final : public Type {
public:
  /// Constructs a NULL canonical type.
  CanType() = default;

public:
  explicit CanType(Type ty) : Type(ty) {
    assert(IsCanTypeOrNull() &&
           "Forming a CanType out of a non-canonical type!");
  }

private:
  bool IsCanTypeOrNull() const { return true; }

public:
  void Visit(llvm::function_ref<void(CanType)> fn) const {
    FindIf([&fn](Type t) -> bool {
      fn(CanType(t));
      return false;
    });
  }
  bool FindIf(llvm::function_ref<bool(CanType)> fn) const {
    return Type::FindIf([&fn](Type t) { return fn(CanType(t)); });
  }
};

} // namespace syn
} // namespace stone

#endif