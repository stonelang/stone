#ifndef STONE_SYNTAX_GLOBAL_H
#define STONE_SYNTAX_GLOBAL_H

#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/PointerUnion.h"

namespace stone {
namespace syn {
class Decl;

class GlobalDecl final {
  llvm::PointerIntPair<const Decl *, 3> pointer;

public:
  GlobalDecl() = default;

  // GlobalDecl(const VarDecl *d);
  // GlobalDecl(const FunctionDecl *d);

  // GlobalDecl(const NameableDecl *d);
  // //GlobalDecl(const BlockDecl *D);

  // GlobalDecl(const ConstructorDecl *d, ConstructorType ty);
  // GlobalDecl(const DestructorDecl *d, DestructorType ty);
};
} // namespace syn
} // namespace stone
#endif