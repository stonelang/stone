#ifndef STONE_SYNTAX_GLOBAL_H
#define STONE_SYNTAX_GLOBAL_H

#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/PointerUnion.h"

namespace stone {
namespace syn {
class Decl;
class VarDecl;
class NameableDecl;
class FunctionDecl;

enum class KernelReferenceKind : unsigned {
  Kernel = 0,
  Stub = 1,
};

class GlobalDecl final {
  llvm::PointerIntPair<const Decl *, 3> val;

private:
  void SetGlobalDecl(const Decl *d) {
    // assert(!llvm::isa<ConstructorDecl>(d) && "Use other ctor with ctor
    // decls!"); assert(!llvm::isa<DestructorDecl>(d) && "Use other ctor with
    // dtor decls!");
    //  assert(!D->hasAttr<CUDAGlobalAttr>() && "Use other ctor with GPU
    //  kernels!");
    val.setPointer(d);
  }

public:
  GlobalDecl() = default;
  GlobalDecl(const VarDecl *d) { SetGlobalDecl(d); }
  GlobalDecl(const FunctionDecl *d) { SetGlobalDecl(d); }
  GlobalDecl(const NameableDecl *d) { SetGlobalDecl(d); }

  // GlobalDecl(const ConstructorDecl *d, ConstructorType ty);
  // GlobalDecl(const DestructorDecl *d, DestructorType ty);
public:
  const Decl *GetDecl() const { return val.getPointer(); }
};
} // namespace syn
} // namespace stone
#endif