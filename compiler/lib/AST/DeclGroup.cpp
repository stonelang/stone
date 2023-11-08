#include "stone/AST/DeclGroup.h"

#include <cassert>
#include <memory>

#include "stone/AST/ASTContext.h"

using namespace stone;

stone::DeclGroup *stone::DeclGroup::Create(ASTContext &tc, Decl **decls,
                                       unsigned numDecls) {
  // assert(numDecls > 1 && "Invalid DeclGroup");
  // unsigned totalSize = totalSizeToAlloc<Decl *>(numDecls);
  // void *mem = tc.Allocate(totalSize, alignof(DeclGroup));
  // new (mem) DeclGroup(numDecls, decls);
  // return static_cast<DeclGroup*>(mem);
  return nullptr;
}

stone::DeclGroup::DeclGroup(unsigned numDecls, Decl **decls)
    : NumDecls(numDecls) {
  // assert(numDecls > 0);
  // assert(decls);
  // std::uninitialized_copy(decls, decls + numDecls, getTrailingObjects<Decl
  // *>());
}
