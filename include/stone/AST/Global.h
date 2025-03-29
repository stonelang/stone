#ifndef STONE_AST_GLOBAL_H
#define STONE_AST_GLOBAL_H

#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/PointerUnion.h"

namespace stone {

class Decl;
class VarDecl;
class NamedDecl;
class FunctionDecl;

enum class KernelReferenceKind : unsigned {
  Kernel = 0,
  Stub = 1,
};

class GlobalDecl final {
  llvm::PointerIntPair<const Decl *, 3> val;

private:
  void SetGlobalDecl(const Decl *d) {
    assert(!llvm::isa<ConstructorDecl>(d) &&
           "Use other constructor with constructor decls!");

    // assert(!llvm::isa<DestructorDecl>(d) && "Use other ctor with
    //  dtor decls!");
    //   assert(!D->hasAttr<CUDAGlobalAttr>() && "Use other ctor with GPU
    //   kernels!");
    val.setPointer(d);
  }

public:
  GlobalDecl() = default;
  GlobalDecl(const VarDecl *d) { SetGlobalDecl(d); }
  GlobalDecl(const FunctionDecl *d) { SetGlobalDecl(d); }
  GlobalDecl(const Decl *d) { SetGlobalDecl(d); }

  // GlobalDecl(const ConstructorDecl *d, ConstructorKind ty);
  // GlobalDecl(const DestructorDecl *d, DestructorKind ty);

public:
  const Decl *GetDecl() const { return val.getPointer(); }
};

} // namespace stone
#endif